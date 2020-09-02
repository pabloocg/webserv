#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_method.h"
#include "wsv_location.h"
#include "wsv_http_message.h"
#include "wsv_client.h"

static int
wsv_http_method_trace_payload(int fd, struct wsv_http_message_s* m)
{
	ssize_t			bytes_written;

	bytes_written = wsv_write(fd, (void*)m->buf_begin, m->content_length);

	wsv_close(fd); /* insert error check */
	if (bytes_written < 0 || m->content_length != (size_t)bytes_written)
	{
		wsv_close(m->ressource_fd); /* insert error check */
		return (WSV_ERROR_500);
	}
	else
		return (WSV_SUCCESS_200);
}

int
wsv_http_method_trace(struct wsv_http_message_s* m, const void* location)
{
	int				pipefd[2];

	(void)location;

	if (wsv_pipe(pipefd) < 0)
		return (WSV_ERROR_500);

	if (wsv_fcntl_int(pipefd[0], F_SETFL, O_NONBLOCK) == -1 ||
		wsv_fcntl_int(pipefd[1], F_SETFL, O_NONBLOCK) == -1)
	{
		wsv_close(pipefd[0]); /* insert error check */
		wsv_close(pipefd[1]); /* insert error check */
		return (WSV_ERROR_500);
	}

	m->tmp_fd = pipefd[1];
	wsv_client_static_content_write_intialize(pipefd[1], (void*)m);
	m->write_static_content_handler = &wsv_http_method_trace_payload;

	m->ressource_fd = pipefd[0];
	m->content_type = "message/http";
	m->content_length = (m->body_begin - m->buf_begin);

	return (WSV_RETRY);
}
