#include "wsv_retval.h"
#include "wsv_string.h"

static int
wsv_error_page_initialize(char* code, char* file, const char** error_pages)
{
	int				status_code;

	status_code = 0;
	while (*code != '\0')
	{
		if (*code >= '0' && *code <= '9')
		{
			status_code = status_code * 10 + (*code - '0');
			if (status_code > 600)
				return (WSV_ERROR);
			++code;
		}
		else
			return (WSV_ERROR);
	}

	switch (status_code)
	{
		case 400:
			error_pages[WSV_ERROR_400] = file;
			break;
		case 404:
			error_pages[WSV_ERROR_404] = file;
			break;
		case 405:
			error_pages[WSV_ERROR_405] = file;
			break;
		case 411:
			error_pages[WSV_ERROR_411] = file;
			break;
		case 413:
			error_pages[WSV_ERROR_413] = file;
			break;
		case 414:
			error_pages[WSV_ERROR_414] = file;
			break;
		case 500:
			error_pages[WSV_ERROR_500] = file;
			break;
		case 501:
			error_pages[WSV_ERROR_501] = file;
		case 505:
			error_pages[WSV_ERROR_505] = file;
	}

	return (WSV_OK);
}

int
wsv_conf_directive_error_page(char* value_begin,
							  char* value_end,
							  const char** error_pages)
{
	char*			token;
	char*			file;

	--value_end;
	while (value_begin != value_end && *value_end == ' ')
		--value_end;
	if (value_begin == value_end)
		return (WSV_ERROR);

	value_end[1] = '\0';
	while (value_begin != value_end && *value_end != ' ')
		--value_end;

	file = value_end +  1;

	while (value_begin != value_end && *value_end == ' ')
		--value_end;
	if (value_begin == value_end)
		return (WSV_ERROR);
	++value_end;

	do
	{
		token = wsv_str_token(value_begin, value_end, &value_begin, ' ');
		if (wsv_error_page_initialize(token, file, error_pages) == WSV_ERROR)
			return (WSV_ERROR);
	}
	while (value_begin < value_end);

	return (WSV_OK);
}
