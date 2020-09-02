#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_client.h"

int
wsv_http_response_ressource_read(int fd, struct wsv_http_message_s* m)
{
	ssize_t			bytes_read;

	bytes_read = wsv_read(fd, (void*)m->buf_begin, (size_t)m->buf_size);

	if (bytes_read == 0)
		return (WSV_OK);
	else
	{
		m->buf_end = m->buf_begin + bytes_read;
		wsv_client_send_initialize(fd, (void*)m);
		return (WSV_RETRY);
	}
}

int
wsv_http_response_write(int sockfd, struct wsv_http_message_s* m)
{
	enum states
	{
		status_line_headers = 0x0,
		body_send = 0x1,
		cgi_send = 0x2
	};
	ssize_t			bytes_received;
	size_t			count;

	switch ((enum states)m->state_response)
	{
	case status_line_headers:
		bytes_received = send(sockfd,
							  (void*)m->buf_begin,
							  (m->buf_end - m->buf_begin),
							  MSG_DONTWAIT);
		if (bytes_received > 0)
		{
			if (m->ressource_fd >= 0)
			{
				wsv_client_static_content_read_intialize(m->ressource_fd,
														 (void*)m);
				m->state_response = body_send;
				return (WSV_RETRY);
			}
			else
				return (WSV_OK);
		}
		else
			return (WSV_ERROR);

	case body_send:
		bytes_received = wsv_send(sockfd,
							  (void*)m->buf_begin,
							  (m->buf_end - m->buf_begin),
							  MSG_DONTWAIT);
		if (bytes_received > 0)
		{
			wsv_client_static_content_read_intialize(m->ressource_fd, (void*)m);
			return (WSV_RETRY);
		}
		else
			return (WSV_ERROR);

	case cgi_send:
		count = (m->cgi_end - m->cgi_begin);
		if (count > BUF_SIZE)
			count = BUF_SIZE;
		bytes_received = wsv_send(sockfd,
								  m->cgi_begin,
								  count,
								  MSG_DONTWAIT);
		if (bytes_received >= 0)
		{
			m->cgi_begin += bytes_received;
			if (m->cgi_begin == m->cgi_end)
				return (WSV_OK);
			else
				return (WSV_RETRY);
		}
		else
			return (WSV_ERROR);
	}
}
