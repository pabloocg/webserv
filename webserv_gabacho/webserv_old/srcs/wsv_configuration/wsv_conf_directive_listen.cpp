#include "wsv_retval.h"
#include "wsv_internet_protocol.h"

int
wsv_conf_directive_listen(char* value_begin,
						  char* value_end,
						  unsigned char ipv6_addr[16],
						  unsigned char port[2])
{
	char*			ipv6_begin;
	char*			ipv6_end;
	char*			port_begin;
	char*			port_end;

	--value_end;
	while (value_begin != value_end && *value_end == ' ')
		--value_end;
	++value_end;

	if (*value_begin != '[')
		return (WSV_ERROR);

	ipv6_begin = ++value_begin;
	while (value_begin != value_end && *value_begin != ']')
		++value_begin;
	if (value_begin == value_end)
		return (WSV_ERROR);
	ipv6_end = value_begin;

	++value_begin;
	if (value_begin == value_end)
	{
		return (wsv_ipv6_binary_form(ipv6_begin, ipv6_end, ipv6_addr));
	}
	if (*value_begin == ':')
	{
		port_begin = ++value_begin;
		port_end = value_end;

		if (wsv_ipv6_binary_form(ipv6_begin, ipv6_end, ipv6_addr) == WSV_OK)
			return (wsv_port_binary_form(port_begin, port_end, port));
	}

	return (WSV_ERROR);
}
