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

#define FLAGS		(O_WRONLY | O_CREAT | O_TRUNC | O_NOFOLLOW | O_NONBLOCK)
#define MODE		(00666)

static int
wsv_http_method_put_payload_chunked(int fd, struct wsv_http_message_s* m)
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

	if (m->content_length > m->max)
	{
		wsv_close(m->ressource_fd); /* insert error check */
		return (WSV_ERROR_413);
	}

	if (m->payload_size == 0)
		m->state_payload += 1;

	return (wsv_http_request_body_chunked_parse(m));
}

static int
wsv_http_method_put_payload(int fd, struct wsv_http_message_s* m)
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
		wsv_close(m->ressource_fd); /* insert error check */
		m->ressource_fd = -1;
		wsv_mime_type(m->abs_path_extension, &m->content_type);
		return (m->status_code);
	}
	else
	{
		wsv_client_receive_initialize(fd, (void*)m);
		return (WSV_RETRY);
	}
}

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
wsv_http_method_put(struct wsv_http_message_s* m, const void* location)
{
	const struct wsv_location_s*	lc;
	const char*						target_ressource;
	char*							field_value;
	struct stat						statbuf;

	lc = (const struct wsv_location_s*)location;

	wsv_http_method_target_ressource_3(&m->target_ressource,
		m->absolute_path, lc->pattern, lc->root, lc->index);
	target_ressource = m->target_ressource.c_str();

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

		m->read_request_handler = &wsv_http_request_body_chunked_read;
		m->write_static_content_handler = &wsv_http_method_put_payload_chunked;

		if (wsv_stat(target_ressource, &statbuf) < 0)
		{
			m->status_code = WSV_SUCCESS_200;
			m->location = m->absolute_path;
		}
		else
			m->status_code = WSV_SUCCESS_200;

		m->ressource_fd = wsv_open_m(target_ressource, FLAGS, MODE);
		if (m->ressource_fd < 0)
			return (WSV_ERROR_500);

		m->max = lc->client_max_body_size;

		return (wsv_http_request_body_chunked_parse(m));
	}
	else if (m->headers[CONTENT_LENGTH][0] != '\0')
	{
		if (wsv_str_to_size(m->headers[CONTENT_LENGTH],
							&m->content_length) == WSV_ERROR)
			return (WSV_ERROR_400);

		if (m->content_length > lc->client_max_body_size)
			return (WSV_ERROR_413);

		m->read_request_handler = &wsv_http_request_body_read;
		m->write_static_content_handler = &wsv_http_method_put_payload;
		m->payload_size = m->content_length;

		if (wsv_stat(target_ressource, &statbuf) < 0)
		{
			m->status_code = WSV_SUCCESS_201;
			m->location = m->absolute_path;
		}
		else
			m->status_code = WSV_SUCCESS_201;

		m->ressource_fd = wsv_open_m(target_ressource, FLAGS, MODE);
		if (m->ressource_fd < 0)
			return (WSV_ERROR_500);

		if (m->content_length == 0)
		{
			wsv_close(m->ressource_fd); /* insert error check */
			m->ressource_fd = -1;
			wsv_mime_type(m->abs_path_extension, &m->content_type);
			return (m->status_code);
		}

		if (m->body_begin != m->body_end)
			wsv_client_static_content_write_intialize(m->ressource_fd,
													  (void*)m);

		return (WSV_RETRY);
	}
	else
		return (WSV_ERROR_411);
}
