#include <stdint.h>

#include "wsv_retval.h"
#include "wsv_internet_protocol.h"

void
wsv_port_text_form(const unsigned char port[2], char dst[6])
{
	uint16_t	nb;
	char		dst_reverse[6];
	char*		it;

	nb = (uint16_t)((port[0] << 8) + port[1]);

	it = dst_reverse;
	*it = '\0';

	do
	{
		++it;
		*it = (char)('0' + (nb % 10));
		nb /= 10;
	} while (nb != 0);

	while ((*dst = *it) != '\0')
	{
		++dst;
		--it;
	}
}

int
wsv_port_binary_form(const char* first,
					 const char* last,
					 unsigned char port[2])
{
	unsigned int	nb;

	if (first == last)
		return (WSV_ERROR);

	nb = 0;
	while (first != last)
	{
		if (*first >= '0' && *first <= '9')
		{
			nb = nb * 10 + (*first - '0');
			if (nb > 0xffff)
				return (WSV_ERROR);
			++first;
		}
		else
			return (WSV_ERROR);
	}

	port[0] = (nb & 0xff00) >> 8;
	port[1] = (nb & 0xff);

	return (WSV_OK);
}
