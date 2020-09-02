#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_http_header.h"
#include "wsv_http_special_characters.h"
#include "wsv_mime_type.h"
#include "wsv_string.h"
#include "wsv_location.h"
#include "wsv_http_method.h"
#include "wsv_cgi.h"
#include "wsv_client.h"
#include "wsv_configuration.h"

#define FLAGS		(O_RDWR | O_CREAT | O_TRUNC | O_NOFOLLOW)
#define MODE		(00666)

static const char*	wsv_http_methods_names[] =
{
	0,
	"GET",
	"HEAD",
	"POST",
	"PUT",
	"DELETE",
	"CONNECT",
	"OPTIONS",
	"TRACE"
};

static int
wsv_str_to_size(char* str, size_t* dst)
{
	size_t					size;

	while (*str != '\0' && *str == ' ')
		++str;
	if (*str == '\0')
		return (WSV_ERROR);

	size = 0;
	while (*str >= '0' && *str <= '9')
	{
		size = size * 10 + (*str - '0');
		if (size > SIZE_MAX)
			return (WSV_ERROR);
		++str;
	}
	*dst = size;

	while (*str != '\0' && *str == ' ')
		++str;
	if (*str == '\0')
		return (WSV_OK);
	else
		return (WSV_ERROR);
}


int
wsv_http_request_body_chunked_parse_cgi(struct wsv_http_message_s* m)
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
				m->state_payload = chunk_data_write;
			}
			else
				goto error_400;
			break;

		case chunk_data_write:
			m->body_begin = it;
			wsv_client_static_content_write_intialize(m->cgi_input, (void*)m);
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

	wsv_client_receive_initialize(m->cgi_input, (void*)m);
	return (WSV_RETRY);

error_400:
	wsv_close(m->cgi_input); /* insert error check */
	return (WSV_ERROR_400);

done:
	wsv_mime_type(m->abs_path_extension, &m->content_type);

	std::string temp = std::string(m->absolute_path);

	if (wsv_lseek(m->cgi_input, 0, SEEK_SET) == ((off_t)-1))
			return (WSV_ERROR_500);

	return (wsv_cgi(m->headers,
					m->client_ipv6_addr_str,
					(char*)m->cgi_executable.c_str(),
					(char*)wsv_http_methods_names[m->method],
					temp,
					(char*)m->target_ressource.c_str(),
					(char*)m->query.c_str(),
					m->cgi_input,
					m));
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
		wsv_close(fd);
		return (WSV_ERROR_500);
	}

	m->body_begin += bytes_written;
	m->content_length += bytes_written;
	m->payload_size -= bytes_written;

	if (m->payload_size == 0)
		m->state_payload += 1;

	return (wsv_http_request_body_chunked_parse_cgi(m));
}


int
wsv_http_request_body_chunked_read_cgi(int sockfd, struct wsv_http_message_s* m)
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

		return (wsv_http_request_body_chunked_parse_cgi(m));
	}
	else
	{
		wsv_close(m->cgi_input);
		return (WSV_ERROR);
	}
}

int
wsv_http_request_body_read_cgi(int sockfd, struct wsv_http_message_s* m)
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

		wsv_client_static_content_write_intialize(m->cgi_input, (void*)m);
		return (WSV_RETRY);
	}
	else
	{
		wsv_close(m->cgi_input);
		return (WSV_ERROR);
	}
}

static int
wsv_http_method_cgi_payload(int fd, struct wsv_http_message_s* m)
{
	size_t			count;
	ssize_t			bytes_written;
	std::string temp;

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
		wsv_mime_type(m->abs_path_extension, &m->content_type);
		temp = std::string(m->absolute_path);

		if (wsv_lseek(fd, 0, SEEK_SET) == ((off_t)-1))
			return (WSV_ERROR_500);

		return (wsv_cgi(m->headers,
						m->client_ipv6_addr_str,
						(char*)m->cgi_executable.c_str(),
						(char*)wsv_http_methods_names[m->method],
						temp,
						(char*)m->target_ressource.c_str(),
						(char*)m->query.c_str(),
						fd,
						m));
	}
	else
	{
		wsv_client_receive_initialize(fd, (void*)m);
		return (WSV_RETRY);
	}
}

int
wsv_http_method_cgi(struct wsv_http_message_s* m, const void* location,
					std::string& cgiExecutable)
{
	const struct wsv_location_s*	lc;
	char*							target_ressource;
	char*							field_value;
	struct stat						statbuf;

	lc = (const struct wsv_location_s*)location;
	m->cgi_executable = cgiExecutable;
	wsv_http_method_target_ressource_2(&m->target_ressource,
		m->absolute_path, lc->pattern, lc->root);
	target_ressource = (char*) m->target_ressource.c_str();

	size_t pos = m->target_ressource.find('.');
	if (pos == std::string::npos)
		m->abs_path_extension = (char*)"";
	else
		m->abs_path_extension = (char*)(m->target_ressource.c_str() + pos);

	if (m->headers[TRANSFER_ENCODING][0] != '\0')
	{
		field_value = wsv_str_trim(m->headers[TRANSFER_ENCODING], ' ');
		if (wsv_strcmp(field_value, "chunked") != 0)
			return (WSV_ERROR_501);

		m->read_request_handler = &wsv_http_request_body_chunked_read_cgi;
		m->write_static_content_handler = &wsv_http_method_cgi_payload_chunked;

		if (wsv_stat(target_ressource, &statbuf) < 0)
		{
			m->status_code = WSV_SUCCESS_201;
			m->location = m->absolute_path;
		}
		else
			m->status_code = WSV_SUCCESS_200;

		if ((m->cgi_input = wsv_open_m("cgi_input.tmp", FLAGS, MODE)) < 0)
			return (WSV_ERROR_500);

		return (wsv_http_request_body_chunked_parse_cgi(m));
	}
	else if (m->headers[CONTENT_LENGTH][0] != '\0')
	{
		if (wsv_str_to_size(m->headers[CONTENT_LENGTH],
							&m->content_length) == WSV_ERROR)
			return (WSV_ERROR_400);

		if (m->content_length > lc->client_max_body_size)
			return (WSV_ERROR_413);

		m->read_request_handler = &wsv_http_request_body_read_cgi;
		m->write_static_content_handler = &wsv_http_method_cgi_payload;
		m->payload_size = m->content_length;

		if (wsv_stat(target_ressource, &statbuf) < 0)
		{
			m->status_code = WSV_SUCCESS_201;
			m->location = m->absolute_path;
		}
		else
			m->status_code = WSV_SUCCESS_200;

		if ((m->cgi_input = wsv_open_m("cgi_input.tmp", FLAGS, MODE)) < 0)
			return (WSV_ERROR_500);

		if (m->body_begin != m->body_end)
			wsv_client_static_content_write_intialize(m->cgi_input,
													  (void*)m);

		return (WSV_RETRY);
	}
	else
		return (WSV_ERROR_411);
}
