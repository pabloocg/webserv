#include "wsv_retval.h"
#include "wsv_http_message.h"

void
wsv_http_response(struct wsv_http_message_s* m)
{
	m->buf_end = m->buf_begin;
	if (WSV_IS_HTTP_ERROR(m->status_code))
	{
		wsv_http_response_error(m);
	}
	else
	{
		wsv_http_response_status_line(m->status_code, m);
		wsv_http_response_headers(m);
	}
}
