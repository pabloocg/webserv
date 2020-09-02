#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_http_method.h"
#include "wsv_http_special_characters.h"
#include "wsv_string.h"

static int
wsv_http_method(char* str, char** endstr)
{
	if (wsv_str_cmp_4(str, "GET "))
	{
		*endstr = str + 4;
		return (HTTP_METHOD_GET);
	}
	else if (wsv_str_cmp_4(str, "PUT "))
	{
		*endstr = str + 4;
		return (HTTP_METHOD_PUT);
	}
	else if (wsv_str_cmp_5(str, "HEAD "))
	{
		*endstr = str + 5;
		return (HTTP_METHOD_HEAD);
	}
	else if (wsv_str_cmp_5(str, "POST "))
	{
		*endstr = str + 5;
		return (HTTP_METHOD_POST);
	}
	else if (wsv_str_cmp_6(str, "TRACE "))
	{
		*endstr = str + 6;
		return (HTTP_METHOD_TRACE);
	}
	else if (wsv_str_cmp_7(str, "DELETE "))
	{
		*endstr = str + 7;
		return (HTTP_METHOD_DELETE);
	}
	else if (wsv_str_cmp_8(str, "OPTIONS "))
	{
		*endstr = str + 8;
		return (HTTP_METHOD_OPTIONS);
	}
	/*else if (wsv_str_cmp_8(str, "CONNECT "))
	{
		*endstr = str + 8;
		return (HTTP_METHOD_CONNECT);
	}*/
	else
		return (HTTP_METHOD_UNKNOWN);
}

int
wsv_http_request_line_parse(struct wsv_http_message_s* m)
{
	enum states
	{
		method = 0,
		request_target,
		scheme,
		host,
		port,
		absolute_path,
		query,
		http_version,
		crlf,
		done
	};
	char*			it;

	it = m->buf_position;

	for (;;)
	{
		switch ((enum states)m->state_request_line)
		{
		case method: /* ============ METHOD ============ */
			if (*it == CR || *it == LF)
				++it;
			else if ((m->buf_end - it) < 8)
			{
				m->buf_position = it;
				return (WSV_RETRY);
			}
			else
			{
				m->method = wsv_http_method(it, &it);
				if (m->method == HTTP_METHOD_UNKNOWN)
					return (WSV_ERROR);

				m->buf_position = it;
				m->state_request_line = request_target;
			}
			break; /* ------------ METHOD ------------ */

		case request_target: /* ============ REQUEST-TARGET ============ */
			while (it != m->buf_end && *it == ' ')
				++it;
			if (it == m->buf_end)
				return (WSV_RETRY);

			switch (*it)
			{
			case '/':
				m->buf_position = it;
				m->state_request_line = absolute_path;
				break;

			/*case '*':
				m->absolute_path_begin = it;
				m->absolute_path_end = ++it;

				if (it == m->buf_end)
					break;
				else if (*it != ' ')
					return (WSV_ERROR_400);
				else
				{
					m->buf_position = it;
					m->state_request_line = http_version;
				}
				break;*/

			default:
				m->buf_position = it;
				m->state_request_line = scheme;
			}
			break; /* ------------ REQUEST-TARGET ------------ */

		case scheme: /* ============ SCHEME ============ */
			while (it != m->buf_end && *it != '/')
				++it;

			if (it == m->buf_end || ++it == m->buf_end)
				return (WSV_RETRY);
			else if (*it != '/')
				return (WSV_ERROR_400);
			else
			{
				switch (it - m->buf_position)
				{
				case 6:
					if (wsv_str_cmp_4(m->buf_position, "http"))
						m->scheme = SCHEME_HTTP;
					else
						return (WSV_ERROR_400);
					break;

				case 7:

					if (wsv_str_cmp_5(m->buf_position, "https"))
						m->scheme = SCHEME_HTTPS;
					else
						return (WSV_ERROR_400);
					break;

				default:
					return (WSV_ERROR_400);

				}
				m->buf_position = ++it;
				m->state_request_line = host;
			}
			break; /* ------------ SCHEME ------------ */

		case host: /* ============ HOST ============ */
			m->host_begin = it;

			while (it != m->buf_end)
			{
				if (*it == ':')
				{
					m->host_end = it;

					m->buf_position = it;
					m->state_request_line = port;
					break;
				}
				else if (*it == '/')
				{
					m->host_end = it;

					m->buf_position = it;
					m->state_request_line = absolute_path;
					break;
				}
				else if (*it == ' ')
				{
					m->host_end = it;

					m->buf_position = it;
					m->state_request_line = http_version;
					break;
				}
				else
					++it;
			}
			if (it == m->buf_end)
				return (WSV_RETRY);
			break; /* ------------ HOST ------------ */

		case port: /* ============ PORT ============ */
			if (++it == m->buf_end)
				return (WSV_RETRY);
			m->port_begin = it;

			while (it != m->buf_end && (*it >= '0' && *it <= '9'))
				++it;

			if (it == m->buf_end)
				break;
			else if (*it == '/')
			{
				m->port_end = it;

				m->buf_position = it;
				m->state_request_line = absolute_path;
			}
			else if (*it == ' ')
			{
				m->port_end = it;

				m->buf_position = it;
				m->state_request_line = http_version;
			}
			else
				return (WSV_ERROR_400);
			break; /* ------------ PORT ------------ */

		case absolute_path: /* ============ ABSOLUTE-PATH ============ */
			m->absolute_path_begin = it;
			while (it != m->buf_end && (*it != ' ' && *it != '?'))
				++it;

			if (it == m->buf_end)
				return (WSV_RETRY);
			else
			{
				m->absolute_path_end = it;

				m->buf_position = it;
				m->state_request_line = (*it == '?' ? query : http_version);
			}
			break; /* ------------ ABSOLUTE-PATH ------------ */

		case query: /* ============ QUERY ============ */
			if (++it == m->buf_end)
				break;
			m->query_begin = it;

			while (it != m->buf_end && *it != ' ')
				++it;

			if (it == m->buf_end)
				return (WSV_RETRY);
			else
			{
				m->query_end = it;

				m->buf_position = it;
				m->state_request_line = http_version;
			}
			break; /* ------------ QUERY ------------ */

		case http_version: /* ============ HTTP-VERSION ============ */
			while (it != m->buf_end && *it == ' ')
				++it;

			if (it == m->buf_end)
				return (WSV_RETRY);
			else
			{
				int flag;
				m->buf_position = it;

				flag = 0x1;
				while (it != m->buf_end)
				{
					if (flag & 0x1 && *it == 'H')
						flag = 0x2;
					else if (flag & 0x2 && *it == 'T')
						flag = 0x4;
					else if (flag & 0x4 && *it == 'T')
						flag = 0x8;
					else if (flag & 0x8 && *it == 'P')
						flag = 0x10;
					else if (flag & 0x10 && *it == '/')
						flag = 0x20;
					else if (flag & 0x20 && *it == '1')
						flag = 0x40;
					else if (flag & 0x40 && *it == '.')
						flag = 0x80;
					else if (flag & 0x80 && *it == '1')
					{
						m->http_version = HTTP_1_1;

						m->buf_position = ++it;
						m->state_request_line = crlf;
						break;
					}
					else
						return (WSV_ERROR_505);

					++it;
				}
				if (it == m->buf_end)
					break;
			}
			break; /* ------------ HTTP-VERSION ------------ */

		case crlf: /* ============ CRLF ============ */
			while (it != m->buf_end && (*it == ' ' || *it == CR))
				++it;

			if (it == m->buf_end)
				return (WSV_RETRY);
			else if (*it != LF)
				return (WSV_ERROR_400);
			else
			{
				m->buf_position = it;
				m->state_request_line = done;
			}
			break; /* ------------ CRLF ------------ */

		case done:
			if (++it == m->buf_end)
				return (WSV_RETRY);
			else
			{
				m->buf_position = it;
				return (WSV_OK);
			}
		}
	}
}
