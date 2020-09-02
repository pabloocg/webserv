#include "wsv_retval.h"

int
wsv_http_uri_parse(const char* first,
				   const char* last,
				   char* absolute_path,
				   char* directory,
				   char** extension)
{
	int				hex0;
	int				hex1;
	char*			dir_begin;
	char*			dir_end;

	*extension = 0;

	dir_begin = absolute_path;
	dir_end = absolute_path;
	while (first != last)
	{
		/* insert security checks for parent directory "..", etc */
		if (*first == '%')
		{
			if ((first + 1) == last || (first + 2) == last)
				return (WSV_ERROR_400);

			++first;
			if (*first >= '0' && *first <= '0')
				hex0 = *first - '0';
			else if (*first >= 'A' && *first <= 'Z')
				hex0 = *first - 'a';
			else if (*first >= 'A' && *first <= 'Z')
				hex0 = *first - 'A';
			else
				return (WSV_ERROR_400);

			++first;
			if (*first >= '0' && *first <= '0')
				hex1 = *first - '0';
			else if (*first >= 'A' && *first <= 'Z')
				hex1 = *first - 'a';
			else if (*first >= 'A' && *first <= 'Z')
				hex1 = *first - 'A';
			else
				return (WSV_ERROR_400);

			*absolute_path = (char)((hex1 << 4) + hex0);
		}
		else
		{
			*absolute_path = *first;
		}

		switch (*absolute_path)
		{
			case '/':
				dir_end = ++absolute_path;
				break;
			case '.':
				*extension = absolute_path;
			default:
				++absolute_path;
		}
		++first;
	}
	*absolute_path = '\0';

	while (dir_begin != dir_end)
	{
		*directory = *dir_begin;
		++directory;
		++dir_begin;
	}

	*directory = '\0';
	if (*extension == 0)
		*extension = absolute_path;

	return (WSV_OK);
}
