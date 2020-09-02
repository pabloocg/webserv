#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_client.h"

int
wsv_http_response_ressource_read(int fd, struct wsv_http_message_s* m)
{
	ssize_t			bytes_read;

	bytes_read = wsv_read(fd, (void*)m->buf_begin, (size_t)m->buf_size);


	if (bytes_read < 0)
	{
		wsv_close(fd); /* insert error check */
		return (WSV_ERROR);
	}
	else if (bytes_read == 0)
	{
		wsv_close(fd); /* insert error check */
		return (WSV_OK);
	}
	else
	{
		m->buf_end = m->buf_begin + bytes_read;
		wsv_client_write_initialize(fd, (void*)m);
		return (WSV_RETRY);
	}
}
