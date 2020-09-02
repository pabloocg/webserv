#include "wsv_retval.h"

int
wsv_conf_directive_string(char* first, char* last, const char** dst)
{
	--last;
	while (first != last && *last == ' ')
		--last;

	last[1] = '\0';

	*dst = first;

	return (WSV_OK);
}
