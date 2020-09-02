#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_client.h"

int
wsv_http_request_body_chunked_read(int sockfd, struct wsv_http_message_s* m)
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

		return (wsv_http_request_body_chunked_parse(m));
	}
	else
		return (WSV_ERROR);
}

int
wsv_http_request_body_read(int sockfd, struct wsv_http_message_s* m)
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

		wsv_client_static_content_write_intialize(m->ressource_fd, (void*)m);
		return (WSV_RETRY);
	}
	else
		return (WSV_ERROR);
}
