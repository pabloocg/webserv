#include "wsv_retval.h"
#include "wsv_string.h"
#include "wsv_http_special_characters.h"
#include "wsv_time.h"
#include "wsv_http_message.h"
#include "wsv_configuration.h"

static void
wsv_http_response_headers_allow(struct wsv_http_message_s* m)
{
	static const char	wsv_allow[9] = "Allow:  ";
	static const char*	wsv_http_methods[] =
	{
		0,
		"GET",
		"HEAD",
		"POST",
		"PUT",
		"DELETE",
		"CONNECT",
		"OPTIONS",
		"TRACE"
	}; /* -- wsv_http_method.h -- */
	int					limit_except;
	int					method;
	int					n;

	limit_except = (m->limit_except & ~(1));

	wsv_str_cpy_8(m->buf_end, wsv_allow);
	m->buf_end += 6;
	method = 1;
	while (limit_except != 0)
	{
		if (limit_except & (1 << method))
		{
			m->buf_end += 2;
			n = 0;
			while ((*m->buf_end = wsv_http_methods[method][n]) != '\0')
			{
				++m->buf_end;
				++n;
			}
			wsv_str_cpy_2(m->buf_end, ", ");
			limit_except -= (1 << method);
		}
		++method;
	}
	wsv_str_cpy_2(m->buf_end, CRLF);
	m->buf_end += 2;
}

static void
wsv_http_response_headers_location(struct wsv_http_message_s* m)
{
	static const char	wsv_location[11] = "Location: ";
	static const char	wsv_content_location[19] = "Content-Location: ";
	const char*			uri;

	uri = m->location;
	if (uri != 0)
	{
		wsv_str_cpy_10(m->buf_end, wsv_location);
		m->buf_end += 10;
		while ((*m->buf_end = *uri) != '\0')
		{
			++m->buf_end;
			++uri;
		}
		wsv_str_cpy_2(m->buf_end, CRLF);
		m->buf_end += 2;
	}

	uri = m->content_location;
	if (uri != 0)
	{
		wsv_str_cpy_18(m->buf_end, wsv_content_location);
		m->buf_end += 18;
		while ((*m->buf_end = *uri) != '\0')
		{
			++m->buf_end;
			++uri;
		}
		wsv_str_cpy_2(m->buf_end, CRLF);
		m->buf_end += 2;
	}
}

static void
wsv_http_response_headers_time(struct wsv_http_message_s* m)
{
	static const char	wsv_date[7] = "Date: ";
	static const char	wsv_last_modified[16] = "Last-Modified: ";
	struct wsv_time_s	date;

	if (wsv_timeofday(&date) == WSV_OK)
	{
		wsv_str_cpy_6(m->buf_end, wsv_date);
		m->buf_end += 6;
		wsv_date_internet_message_format(&date, m->buf_end);
		m->buf_end += 29;
		wsv_str_cpy_2(m->buf_end, CRLF);
		m->buf_end += 2;
	}

	if (m->last_modified != ((time_t) -1))
	{
		wsv_str_cpy_15(m->buf_end, wsv_last_modified);
		m->buf_end += 15;
		wsv_time_date(m->last_modified, &date);
		wsv_date_internet_message_format(&date, m->buf_end);
		m->buf_end += 29;
		wsv_str_cpy_2(m->buf_end, CRLF);
		m->buf_end += 2;
	}
}

static void
wsv_http_response_headers_content(struct wsv_http_message_s* m)
{
	static const char	wsv_content_type[15] =  "Content-Type: ";
	static const char	wsv_content_length[17] = "Content-Length: ";
	const char*			content_type;
	size_t				content_length;
	int					n;
	struct wsv_server_s* srv;

	content_type = m->content_type;
	content_length = m->content_length;

	srv = (struct wsv_server_s*)m->server;

	if (content_type != 0 &&
		! (
			(srv->cgi[std::string(m->abs_path_extension)] != "") &&
			!WSV_IS_HTTP_ERROR(m->status_code)
		 ))
	{
		wsv_str_cpy_14(m->buf_end, wsv_content_type);
		m->buf_end += 14;
		while ((*m->buf_end = *content_type) != '\0')
		{
			++m->buf_end;
			++content_type;
		}

		wsv_str_cpy_2(m->buf_end, CRLF);
		m->buf_end += 2;
	}

	wsv_str_cpy_16(m->buf_end, wsv_content_length);
	m->buf_end += 16;
	n = 0;
	do
	{
		m->buf_end[n] = (char)('0' + (content_length % 10));
		content_length /= 10;
		++n;
	} while (content_length != 0);
	wsv_str_reverse(m->buf_end, m->buf_end + n);

	wsv_str_cpy_2(m->buf_end + n, CRLF);

	m->buf_end += (n + 2);
}

void
wsv_http_response_headers(struct wsv_http_message_s* m)
{
	static const char	wsv_server[25] = "Server: webserv/draft " CRLF;

	wsv_str_cpy_24(m->buf_end, wsv_server);
	m->buf_end += 24;

	wsv_http_response_headers_content(m);
	wsv_http_response_headers_time(m);
	wsv_http_response_headers_location(m);
	wsv_http_response_headers_allow(m);

	wsv_str_cpy_2(m->buf_end, CRLF);
	m->buf_end += 2;
}
