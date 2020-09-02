#include <limits.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_client.h"

int		wsv_http_request_line_parse(struct wsv_http_message_s* m);
int		wsv_http_uri_parse(const char* first,
						   const char* last,
						   char* absolute_path,
						   char* directory,
						   char** extension);
int		wsv_http_header_field_parse(struct wsv_http_message_s* m);
int		wsv_http_header_field_load(struct wsv_http_message_s* m);

static int
wsv_http_read_request_headers(struct wsv_http_message_s* m)
{
	int				retval;

	while ((retval = wsv_http_header_field_parse(m)) == WSV_OK)
	{
		m->state_headers = 0;
		if (m->header_name_begin == m->header_name_end)
		{
			/* empty line (request line + headers done) */
			m->body_begin = m->buf_position;
			m->body_end = m->buf_end;
			m->buf_end = m->buf_begin;
			retval = wsv_http_request_host(m);
			break;
		}
		else
		{
			retval = wsv_http_header_field_load(m);
			if (retval != WSV_OK)
				break;
		}
	}

	return (retval);
}

static int
wsv_http_read_request_line(struct wsv_http_message_s* m)
{
	int				retval;

	retval = wsv_http_request_line_parse(m);

	if (retval == WSV_OK)
	{
		if (m->absolute_path_begin)
		{
			if ((m->absolute_path_end -
				 m->absolute_path_begin) > (PATH_MAX >> 1))
				return (WSV_ERROR_414);

			m->absolute_path = m->abs_path_root + (PATH_MAX >> 1);

			retval = wsv_http_uri_parse(m->absolute_path_begin,
										m->absolute_path_end,
										m->absolute_path,
										m->abs_path_directory,
										&m->abs_path_extension);
		}
		else
		{
			m->absolute_path = (char*)wsv_malloc(2);
			if (m->absolute_path == 0)
				return (WSV_ERROR_500);

			m->absolute_path[0] = '/';
			m->absolute_path[1] = '\0';

			retval = WSV_OK;
		}
	}

	return (retval);
}

int
wsv_http_request_read(int sockfd, struct wsv_http_message_s* m)
{
	enum states
	{
		request_line = 0,
		headers
	};
	int				retval;
	ssize_t			bytes_received;

	if (m->buf_count > 0)
	{
		bytes_received = wsv_recv(sockfd,
								  (void*)m->buf_end,
								  (size_t)m->buf_count,
								  MSG_DONTWAIT);

		if (bytes_received > 0)
		{
			m->buf_count -= bytes_received;
			m->buf_end += bytes_received;

			switch ((enum states)m->state_request)
			{
			case request_line:
				retval = wsv_http_read_request_line(m);
				if (retval == WSV_OK)
					m->state_request = headers;
				else
					return (retval);

			case headers:
				return (wsv_http_read_request_headers(m));
			}
		}
		else
			return (WSV_ERROR);
	}

	return (WSV_ERROR_414);
}
