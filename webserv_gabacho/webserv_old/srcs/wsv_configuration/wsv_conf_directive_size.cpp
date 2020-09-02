#include <stdint.h>
#include <stddef.h>

#include "wsv_retval.h"

int
wsv_conf_directive_size(char* first, char* last, size_t* dst)
{
	size_t			nb;

	nb = 0;
	while  (first != last && *first != ' ')
	{
		if (*first >= '0' && *first <= '9')
		{
			nb = nb * 10 + (*first - '0');
			if (nb > SIZE_MAX)
				return (WSV_ERROR);
			++first;
		}
		else
			return (WSV_ERROR);
	}

	while (first != last)
	{
		if (*first == ' ')
			++first;
		else
			return (WSV_ERROR);
	}

	*dst = nb;
	return (WSV_OK);
}
