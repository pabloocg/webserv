#include <iostream>

#include <stdint.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_http_header.h"
#include "wsv_http_special_characters.h"
#include "wsv_mime_type.h"
#include "wsv_string.h"
#include "wsv_location.h"
#include "wsv_http_method.h"
#include "wsv_client.h"
#include "wsv_http_cgi.h"
#include "wsv_error_log.h"
#include "wsv_tcp.h"

#define FLAGS		(O_RDWR | O_CREAT | O_TRUNC | O_NOFOLLOW | O_NONBLOCK)
#define MODE		(00666)

/* ------------------------------- Chunked ------------------------------- */
static int
wsv_http_request_cgi_chunked_parse(struct wsv_http_message_s* m)
{
	enum states
	{
		chunk_size_read = 0,
		chunk_size_CRLF = 1,
		chunk_data_write = 2,
		chunk_data_CR = 3,
		chunk_data_LF = 4
	};
	char*			it;

	for (it = m->body_begin; it != m->body_end; ++it)
	{
		switch ((enum states)m->state_payload)
		{
		case chunk_size_read:
			if (*it >= '0' && *it <= '9')
				m->payload_size = (m->payload_size << 4) + (*it - '0');
			else if (*it >= 'a' && *it <= 'z')
				m->payload_size = (m->payload_size << 4) + (*it - 'a' + 10);
			else if (*it >= 'A' && *it <= 'Z')
				m->payload_size = (m->payload_size << 4) + (*it - 'A' + 10);
			else
			{
				switch (*it)
				{
				case CR:
					m->state_payload = chunk_size_CRLF;
					break;
				case LF:
					if (m->payload_size == 0)
						goto done;
					m->payload_max -= m->payload_size;
					if (m->payload_max < 0)
						goto error_413;
					m->state_payload = chunk_data_write;
					break;
				default:
					goto error_400;
				}
				break;
			}
			if (m->payload_size > SIZE_MAX)
				goto error_400;
			break;

		case chunk_size_CRLF:
			if (*it == LF)
			{
				if (m->payload_size == 0)
					goto done;
				m->payload_max -= m->payload_size;
				if (m->payload_max < 0)
					goto error_413;
				m->state_payload = chunk_data_write;
			}
			else
				goto error_400;
			break;

		case chunk_data_write:
			m->body_begin = it;
			wsv_client_static_content_write_intialize(m->cgi_fd, (void*)m);
			return (WSV_RETRY);

		case chunk_data_CR:
			switch (*it)
			{
				case CR:
					m->state_payload = chunk_data_LF;
					break;
				case LF:
					m->state_payload = chunk_size_read;
					break;
				default:
					goto error_400;
			}
			break;

		case chunk_data_LF:
			if (*it == LF)
				m->state_payload = chunk_size_read;
			else
				goto error_400;
		}
	}

	wsv_client_read_initialize(m->cgi_fd, (void*)m);
	return (WSV_RETRY);

error_400:
	wsv_close(m->cgi_fd); /* insert error check */
	return (WSV_ERROR_400);

error_413:
	wsv_close(m->cgi_fd); /* insert error check */
	return (WSV_ERROR_413);

done:
	if (wsv_lseek(m->cgi_fd, 0, SEEK_SET) ==  ((off_t)-1))
	{
		wsv_close(m->cgi_fd); /* insert error check */
		return (WSV_ERROR_500);
	}

	wsv_tcp_multiplexing_clear(m->sockfd, m->fd_sets);
	wsv_tcp_multiplexing_clear(m->cgi_fd, m->fd_sets);
	wsv_tcp_multiplexing_cgi_insert(m->sockfd, m->fd_sets);
	return (WSV_RETRY);
}

static int
wsv_http_request_cgi_chunked_read(int sockfd, struct wsv_http_message_s* m)
{
	ssize_t			bytes_received;

	bytes_received = wsv_recv(sockfd,
							  (void*)m->buf_begin,
							  (size_t)m->buf_size,
							  MSG_DONTWAIT);
	if (bytes_received > 0)
	{
		m->body_begin = m->buf_begin;
		m->body_end = m->body_begin + bytes_received;

		return (wsv_http_request_cgi_chunked_parse(m));
	}
	else
		return (WSV_ERROR);
}

static int
wsv_http_method_cgi_payload_chunked(int fd, struct wsv_http_message_s* m)
{
	size_t			count;
	ssize_t			bytes_written;

	count = (m->body_end - m->body_begin);
	if (count > m->payload_size)
		count = m->payload_size;

	bytes_written = wsv_write(fd, (void*)m->body_begin, count);

	if (bytes_written < 0 || count != (size_t)bytes_written)
	{
		wsv_close(fd); /* insert error check */
		return (WSV_ERROR_500);
	}

	m->body_begin += bytes_written;
	m->content_length += bytes_written;
	m->payload_size -= bytes_written;

	if (m->payload_size == 0)
		m->state_payload += 1;

	return (wsv_http_request_cgi_chunked_parse(m));
}
/* ----------------------------------------------------------------------- */

/* =========================== Content Length =========================== */
static int
wsv_http_request_cgi_read(int sockfd, struct wsv_http_message_s* m)
{
	ssize_t			bytes_received;

	bytes_received = wsv_recv(sockfd,
							  (void*)m->buf_begin,
							  (size_t)m->buf_size,
							  MSG_DONTWAIT);
	if (bytes_received > 0)
	{
		m->body_begin = m->buf_begin;
		m->body_end = m->body_begin + bytes_received;

		wsv_client_static_content_write_intialize(m->cgi_fd, (void*)m);
		return (WSV_RETRY);
	}
	else
		return (WSV_ERROR);
}

static int
wsv_http_method_cgi_payload(int fd, struct wsv_http_message_s* m)
{
	size_t			count;
	ssize_t			bytes_written;

	count = (m->body_end - m->body_begin);
	if (count > m->payload_size)
		count = m->payload_size;

	bytes_written = wsv_write(fd, (void*)m->body_begin, count);

	if (bytes_written < 0 || count != (size_t)bytes_written)
	{
		wsv_close(fd); /* insert error check */
		return (WSV_ERROR_500);
	}

	m->payload_size -= bytes_written;

	if (m->payload_size == 0)
	{
		if (wsv_lseek(fd, 0, SEEK_SET) ==  ((off_t)-1))
		{
			wsv_close(fd); /* insert error check */
			return (WSV_ERROR_500);
		}

		wsv_tcp_multiplexing_clear(m->sockfd, m->fd_sets);
		wsv_tcp_multiplexing_clear(m->cgi_fd, m->fd_sets);
		wsv_tcp_multiplexing_cgi_insert(m->sockfd, m->fd_sets);
		return (WSV_RETRY);
	}
	else
	{
		wsv_client_read_initialize(fd, (void*)m);
		return (WSV_RETRY);
	}
}
/* ====================================================================== */

int
wsv_http_cgi_post(void* http_message, const void* location)
{
	struct wsv_http_message_s*		m;
	const struct wsv_location_s*	lc;
	char*							target_ressource;
	char*							field_value;
	struct stat						statbuf;
	std::string						file_tmp;
	uintptr_t						file_tmp_id;

	m = (struct wsv_http_message_s*)http_message;
	lc = (const struct wsv_location_s*)location;

	target_ressource = wsv_http_method_target_ressource(m->absolute_path,
														m->abs_path_root,
														&m->abs_path_extension,
														lc->pattern,
														lc->root);

	/* ++++ CGI parameters +++ */
	try {
		file_tmp_id = (uintptr_t)m;
		do
		{
			file_tmp.insert(0, 1, (char)('0' + (file_tmp_id % 10)));
			file_tmp_id /= 10;
		} while (file_tmp_id != 0);
		file_tmp.insert(0, "/tmp/wsv_cgi_");
		file_tmp.append(".tmp");
	}
	catch (const std::exception& e) {
		std::cerr << "\033[0;31m"
				  << "wsv_http_cgi_post(): "
				  << e.what()
				  << "\033[0;0m"
				  << std::endl;
		return (WSV_ERROR_500);
	}
	/* +++++++++++++++++++++++ */

	if (m->headers[TRANSFER_ENCODING][0] != '\0')
	{
		field_value = wsv_str_trim(m->headers[TRANSFER_ENCODING], ' ');
		if (wsv_strcmp(field_value, "chunked") != 0)
			return (WSV_ERROR_501);

		m->payload_max = lc->client_max_body_size;

		m->read_request_handler = &wsv_http_request_cgi_chunked_read;
		m->write_static_content_handler = &wsv_http_method_cgi_payload_chunked;

		if (wsv_stat(target_ressource, &statbuf) < 0)
		{
			m->status_code = WSV_SUCCESS_201;
			m->location = m->absolute_path;
		}
		else
			m->status_code = WSV_SUCCESS_200;

		if (S_ISDIR(statbuf.st_mode))
			return (WSV_ERROR_500);

		/* ++++ CGI parameters +++ */
		m->cgi_bin = lc->cgi_bin;
		m->cgi_target_ressource = target_ressource;
		m->cgi_fd = wsv_open_m(file_tmp.c_str(), FLAGS, MODE);
		if (m->cgi_fd < 0)
			return (WSV_ERROR_500);
		/* +++++++++++++++++++++++ */

		return (wsv_http_request_cgi_chunked_parse(m));
	}
	else if (m->headers[CONTENT_LENGTH][0] != '\0')
	{
		if (wsv_str_to_size(m->headers[CONTENT_LENGTH],
							&m->content_length) == WSV_ERROR)
			return (WSV_ERROR_400);

		if (m->content_length > lc->client_max_body_size)
			return (WSV_ERROR_413);

		m->read_request_handler = &wsv_http_request_cgi_read;
		m->write_static_content_handler = &wsv_http_method_cgi_payload;
		m->payload_size = m->content_length;

		if (wsv_stat(target_ressource, &statbuf) < 0)
		{
			m->status_code = WSV_SUCCESS_201;
			m->location = m->absolute_path;
		}
		else
			m->status_code = WSV_SUCCESS_200;

		if (S_ISDIR(statbuf.st_mode))
			return (WSV_ERROR_500);

		/* ++++ CGI parameters +++ */
		m->cgi_bin = lc->cgi_bin;
		m->cgi_target_ressource = target_ressource;
		m->cgi_fd = wsv_open_m(file_tmp.c_str(), FLAGS, MODE);
		if (m->cgi_fd < 0)
			return (WSV_ERROR_500);
		/* +++++++++++++++++++++++ */

		if (m->content_length == 0)
		{
			if (wsv_lseek(m->cgi_fd, 0, SEEK_SET) ==  ((off_t)-1))
			{
				wsv_close(m->cgi_fd); /* insert error check */
				return (WSV_ERROR_500);
			}

				wsv_tcp_multiplexing_clear(m->sockfd, m->fd_sets);
				wsv_tcp_multiplexing_cgi_insert(m->sockfd, m->fd_sets);
				return (WSV_RETRY);
		}

		if (m->body_begin != m->body_end)
			wsv_client_static_content_write_intialize(m->cgi_fd, (void*)m);

		return (WSV_RETRY);
	}
	else
		return (WSV_ERROR_411);
}
