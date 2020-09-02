#include "wsv_string.h"
#include "wsv_http_special_characters.h"
#include "wsv_http_message.h"

void
wsv_http_response_status_line(int status_code, struct wsv_http_message_s* m)
{
	static const char*		wsv_status_line[] =
	{
		0, /* empty cell */
		0, /* empty cell */
		"HTTP/1.1 200 OK" CRLF,
		"HTTP/1.1 201 Created" CRLF,
		"HTTP/1.1 204 No Content" CRLF,
		"HTTP/1.1 400 Bad Request" CRLF,
		"HTTP/1.1 404 Not Found" CRLF,
		"HTTP/1.1 405 Method Not Allowed" CRLF,
		"HTTP/1.1 411 Length Required" CRLF,
		"HTTP/1.1 413 Payload Too Large" CRLF,
		"HTTP/1.1 414 URI Too Long" CRLF,
		"HTTP/1.1 500 Internal Server Error" CRLF,
		"HTTP/1.1 501 Not Implemented" CRLF,
		"HTTP/1.1 505 HTTP Version Not Supported" CRLF
	};
	int						n;

	wsv_str_cpy_12(m->buf_end, wsv_status_line[status_code]);

	n = 12;
	while ((m->buf_end[n] = wsv_status_line[status_code][n]) != '\0')
		++n;

	m->buf_end += n;
}
