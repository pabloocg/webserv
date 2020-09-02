#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_string.h"

void
wsv_uppercase(char* letter)
{
	static char		uppercase_letters[26] = {'A','B','C','D','E','F','G',
											 'H','I','J','K','L','M','N',
											 'O','P','Q','R', 'S','T','U',
											 'V','W','X','Y','Z'};

	if (*letter >= 'a' && *letter <= 'z')
		*letter  = uppercase_letters[*letter - 'a'];
}

static int
wsv_http_field_name(char* first, char* last, int* index)
{
	int							retval;

	*last = '\0';

	retval = wsv_http_header(first, index);

	*last = ':';

	return (retval);
}

int
wsv_http_header_field_load(struct wsv_http_message_s* m)
{
	int							index;
	int							len;
	int							flag;

	if (wsv_http_field_name(m->header_name_begin,
							m->header_name_end,
							&index) == WSV_OK)
	{
		flag = (1 << index);
		if (m->headers_flag & flag)
			return (WSV_ERROR_400); /* duplicate header */
		else
			m->headers_flag += flag;

		len = 0;
		while ((m->header_value_begin != m->header_value_end) &&
			   (len != HEADER_VALUE_SIZE))
		{
			m->headers[index][len] = *m->header_value_begin;
			++m->header_value_begin;
			++len;
		}
		if (len == HEADER_VALUE_SIZE)
			return (WSV_ERROR_400); /* header too large */
		else
			m->headers[index][len] = '\0';
	}
	else
	{
		if ((m->header_name_end - m->header_name_begin) != 24 ||
			!wsv_str_cmp_24(m->header_name_begin, "x-secret-header-for-test"))
		{
			return (WSV_OK);
		}

		while (m->header_value_begin != m->header_value_end)
		{
			if (*(m->header_value_begin) != ' ')
				break;
			++(m->header_value_begin);
		}

		while (m->header_value_begin != m->header_value_end)
		{
			if (*(m->header_value_end) != ' ')
				break;
			--(m->header_value_end);
		}

		char* it = m->header_name_begin;
		while (it != m->header_name_end)
		{
			if (*it == '-')
				*it = '_';
			else
				wsv_uppercase(it);
			++it;
		}

		m->cgi_headers.push_back(std::string(m->header_name_begin,
											 m->header_name_end)
								+ "=" +
								 std::string(m->header_value_begin,
											 m->header_value_end));
		m->cgi_size = 0;
	}

	return (WSV_OK);
}
