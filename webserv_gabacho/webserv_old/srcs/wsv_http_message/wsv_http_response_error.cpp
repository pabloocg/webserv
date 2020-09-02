#include <stdint.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_special_characters.h"
#include "wsv_string.h"
#include "wsv_mime_type.h"
#include "wsv_http_message.h"
#include "wsv_configuration.h"

static char			g_wsv_error_400[] =
{
	"<html>\n"
	"<head><title>400 Bad Request</title></head>\n"
	"<body bgcolor=\"white\">\n"
	"<center><h1>400 Bad Request</h1></center>\n"
	"<hr><center>webserv (.DRAFT VERSION)</center>\n"
	"</body>\n"
	"</html>\n"
};

static char			g_wsv_error_404[] =
{
	"<html>\n"
	"<head><title>404 Not Found</title></head>\n"
	"<body bgcolor=\"white\">\n"
	"<center><h1>404 Not Found</h1></center>\n"
	"<hr><center>webserv (.DRAFT VERSION)</center>\n"
	"</body>\n"
	"</html>\n"
};

static char			g_wsv_error_405[] =
{
	"<html>\n"
	"<head><title>405 Method Not Allowed</title></head>\n"
	"<body bgcolor=\"white\">\n"
	"<center><h1>405 Method Not Allowed</h1></center>\n"
	"<hr><center>webserv (.DRAFT VERSION)</center>\n"
	"</body>\n"
	"</html>\n"
};

static char			g_wsv_error_411[] =
{
	"<html>\n"
	"<head><title>411 Length Required</title></head>\n"
	"<body bgcolor=\"white\">\n"
	"<center><h1>411 Length Required</h1></center>\n"
	"<hr><center>webserv (.DRAFT VERSION)</center>\n"
	"</body>\n"
	"</html>\n"
};

static char			g_wsv_error_413[] =
{
	"<html>\n"
	"<head><title>413 Payload Too Large</title></head>\n"
	"<body bgcolor=\"white\">\n"
	"<center><h1>413 Payload Too Large</h1></center>\n"
	"<hr><center>webserv (.DRAFT VERSION)</center>\n"
	"</body>\n"
	"</html>\n"
};

static char			g_wsv_error_414[] =
{
	"<html>\n"
	"<head><title>414 URI Too Long</title></head>\n"
	"<body bgcolor=\"white\">\n"
	"<center><h1>414 URI Too Long</h1></center>\n"
	"<hr><center>webserv (.DRAFT VERSION)</center>\n"
	"</body>\n"
	"</html>\n"
};

static char			g_wsv_error_500[] =
{
	"<html>\n"
	"<head><title>500 Internal Server Error</title></head>\n"
	"<body bgcolor=\"white\">\n"
	"<center><h1>500 Internal Server Error</h1></center>\n"
	"<hr><center>webserv (.DRAFT VERSION)</center>\n"
	"</body>\n"
	"</html>\n"
};

static char			g_wsv_error_501[] =
{
	"<html>\n"
	"<head><title>501 Not Implemented</title></head>\n"
	"<body bgcolor=\"white\">\n"
	"<center><h1>501 Not Implemented</h1></center>\n"
	"<hr><center>webserv (.DRAFT VERSION)</center>\n"
	"</body>\n"
	"</html>\n"
};

static char			g_wsv_error_505[] =
{
	"<html>\n"
	"<head><title>505 HTTP Version Not Supported</title></head>\n"
	"<body bgcolor=\"white\">\n"
	"<center><h1>505 HTTP Version Not Supported</h1></center>\n"
	"<hr><center>webserv (.DRAFT VERSION)</center>\n"
	"</body>\n"
	"</html>\n"
};

static void
wsv_http_error_default(int status_code, struct wsv_http_message_s* m)
{
	char*			wsv_http_errors[][2] =
	{
		{ 0, 0 }, /* empty cell */
		{ 0, 0 }, /* empty cell */
		{ 0, 0 }, /* empty cell */
		{ 0, 0 }, /* empty cell */
		{ 0, 0 }, /* empty cell */
		{ g_wsv_error_400, g_wsv_error_400 + 178 },
		{ g_wsv_error_404, g_wsv_error_404 + 174 },
		{ g_wsv_error_405, g_wsv_error_405 + 192 },
		{ g_wsv_error_411, g_wsv_error_411 + 186 },
		{ g_wsv_error_413, g_wsv_error_413 + 190 },
		{ g_wsv_error_414, g_wsv_error_414 + 180 },
		{ g_wsv_error_500, g_wsv_error_500 + 198 },
		{ g_wsv_error_501, g_wsv_error_501 + 186 },
		{ g_wsv_error_505, g_wsv_error_505 + 208 }
	};
	char*			first;
	char*			last;

	first = wsv_http_errors[status_code][0];
	last = wsv_http_errors[status_code][1];

	m->ressource_fd = -1;
	m->content_type = "text/html";
	m->content_length = (last - first);
	m->location = 0;

	wsv_http_response_status_line(status_code, m);
	wsv_http_response_headers(m);

	while (first != last) /* slow operation */
	{
		*((uint16_t*)(m->buf_end)) = *((uint16_t*)first);
		m->buf_end += 2;
		first += 2;
	}
}

static void
wsv_http_error_custom(int status_code,
					  struct wsv_http_message_s* m,
					  const char* error_page)
{
	int					fd;
	const char*			extension;
	struct stat			statbuf;

	++error_page;

	wsv_str_extension_find_const(&extension, error_page);

	if (wsv_stat(error_page, &statbuf) < 0 ||
		(statbuf.st_mode & S_IFMT) == S_IFDIR)
	{
		wsv_http_error_default(status_code, m);
		return;
	}

	fd = wsv_open(error_page, O_RDONLY | O_NOFOLLOW | O_NONBLOCK);
	if (fd < 0)
	{
		wsv_http_error_default(status_code, m);
		return;
	}

	m->ressource_fd = fd;
	wsv_mime_type(extension, &m->content_type);
	m->content_length = (size_t)statbuf.st_size;
	m->location = 0;

	wsv_http_response_status_line(status_code, m);
	wsv_http_response_headers(m);
}

void
wsv_http_response_error(struct wsv_http_message_s* m)
{
	int								status_code;
	struct wsv_server_s*			srv;

	status_code = m->status_code;
	srv = (struct wsv_server_s*)m->server;

	if (srv->error_pages[status_code] != 0)
		wsv_http_error_custom(status_code, m, srv->error_pages[status_code]);
	else
		wsv_http_error_default(status_code, m);
}
