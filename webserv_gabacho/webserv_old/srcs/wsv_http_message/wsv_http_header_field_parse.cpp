#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_http_special_characters.h"

static void
wsv_lowercase(char* letter)
{
	static char		lowercase_letters[26] = {'a','b','c','d','e','f','g',
											 'h','i','j','k','l','m','n',
											 'o','p','q','r', 's','t','u',
											 'v','w','x','y','z'};

	if (*letter >= 'A' && *letter <= 'Z')
		*letter  = lowercase_letters[*letter - 'A'];
}

int
wsv_http_header_field_parse(struct wsv_http_message_s* m)
{
	enum states
	{
		field_name = 0,
		colon,
		field_value_begin,
		field_value_end,
		crlf
	};
	char*			it;

	for (it = m->buf_position; it != m->buf_end; ++it)
	{
		switch ((enum states)m->state_headers)
		{
		case field_name: /* ============ FIELD_NAME ============ */
			switch (*it)
			{
			case ':':
				return (WSV_ERROR_400);
			case CR:
				m->header_name_begin = it;
				m->header_name_end = it;
				m->state_headers = crlf;
				break;
			case LF:
				m->header_name_begin = it;
				m->header_name_end = it;
				goto done;
			default:
				wsv_lowercase(it);
				m->header_name_begin = it;
				m->state_headers = colon;
			}
			break; /* ------------ FIELD_NAME ------------ */

		case colon: /* ============ COLON ============ */
			if (*it == ':')
			{
				m->header_name_end = it;
				m->state_headers = field_value_begin;
			}
			else
				wsv_lowercase(it);
			break; /* ------------ COLON ------------ */

		case field_value_begin: /* ========== FIELD_VALUE_BEGIN ========== */
			switch (*it)
			{
			case CR:
				m->header_value_begin = it;
				m->header_value_end = it;
				m->state_headers = crlf;
				break;
			case LF:
				m->header_value_begin = it;
				m->header_value_end = it;
				goto done;
			default:
				m->header_value_begin = it;
				m->state_headers = field_value_end;
			}
			break; /* ---------- FIELD_VALUE_BEGIN ---------- */

		case field_value_end: /* ========== FIELD_VALUE_END ========== */
			switch (*it)
			{
			case CR:
				m->header_value_end = it;
				m->state_headers = crlf;
				break;
			case LF:
				m->header_value_end = it;
				goto done;
			}
			break; /* ---------- FIELD_VALUE_END ---------- */

		case crlf: /* ============ CRLF ============ */
			if (*it != LF)
				return (WSV_ERROR_400);
			else
				goto done;
		/* ------------ CRLF ------------ */
		}
	}

	/*
	** RETRY:
	** +) it == r->buf_end
	** +) Meaning that each new state is always previous position + 1,
	**	except for the first state
	** +) Additionally we restart from the last position checked + 1
	*/
	m->buf_position = it;
	return (WSV_RETRY);

done:
	m->buf_position = it + 1;
	return (WSV_OK);
}
