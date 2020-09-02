#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_client.h"

int
wsv_http_response_write(int sockfd, struct wsv_http_message_s* m)
{
	enum states
	{
		status_line_headers = 0x0,
		body_send = 0x1,
		cgi_send = 0x2
	};
	ssize_t			bytes_sent;
	size_t			count;

	switch ((enum states)m->state_response)
	{
	case status_line_headers:
		bytes_sent = wsv_send(sockfd,
							  (void*)m->buf_begin,
							  (m->buf_end - m->buf_begin),
							  MSG_DONTWAIT);
		if (bytes_sent > 0)
		{

#ifdef WSV_LOG
			write(STDOUT_FILENO, "\n==== Response: ====\n", 21);
			write(STDOUT_FILENO, m->buf_begin, m->buf_end - m->buf_begin);
			write(STDOUT_FILENO, "===================\n", 20);
#endif

			if (m->ressource_fd < 0)
				return (WSV_OK);
			m->state_response = body_send;
			m->read_static_content_handler = &wsv_http_response_ressource_read;
			wsv_client_static_content_read_intialize(m->ressource_fd, (void*)m);
			return (WSV_RETRY);
		}
		else
		{
			if (m->ressource_fd >= 0)
				wsv_close(m->ressource_fd); /* insert error check */
			return (WSV_ERROR);
		}

	case body_send:
		bytes_sent = wsv_send(sockfd,
							  (void*)m->buf_begin,
							  (m->buf_end - m->buf_begin),
							  MSG_DONTWAIT);
		if (bytes_sent > 0)
		{
			wsv_client_static_content_read_intialize(m->ressource_fd, (void*)m);
			return (WSV_RETRY);
		}
		else
		{
			wsv_close(m->ressource_fd); /* insert error check */
			return (WSV_ERROR);
		}

	case cgi_send:
		count = (m->cgi_end - m->cgi_begin);
		if (count > m->buf_size)
			count = m->buf_size;
		bytes_sent = wsv_send(sockfd,
							  m->cgi_begin,
							  count,
							  MSG_DONTWAIT);
		if (bytes_sent > 0)
		{
			m->cgi_begin += bytes_sent;
			if (m->cgi_begin == m->cgi_end)
			{
				m->cgi_string.clear();
				return (WSV_OK);
			}
			else
				return (WSV_RETRY);
		}
		else
		{
			m->cgi_string.clear();
			return (WSV_ERROR);
		}
	}
}
