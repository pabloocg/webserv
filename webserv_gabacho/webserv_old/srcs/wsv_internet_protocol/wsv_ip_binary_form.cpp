#include "wsv_retval.h"
#include "wsv_string.h"

/*
** NOTES:
** +) Network byte order (big endian, Most Significant Byte first)
*/

int
wsv_ipv4_binary_form(const char* first,
					 const char* last,
					 unsigned char ipv4_addr[4])
{
	int			n;
	int			decimal;
	int			saw_digit;

	n = 0;
	decimal = 0;
	saw_digit = 0;
	while (first != last)
	{
		if (*first >= '0' && *first <= '9')
		{
			decimal = (decimal * 10) + (*first - '0');
			if (decimal > 255)
				return (WSV_ERROR);
			saw_digit = 1;

			++first;
		}
		else if (*first == '.' && saw_digit)
		{
			ipv4_addr[n] = (unsigned char)decimal;
			++n;
			if (n == 4)
				return (WSV_ERROR);
			decimal = 0;
			saw_digit = 0;

			++first;
		}
		else
			return (WSV_ERROR);
	}

	ipv4_addr[n] = (unsigned char)decimal;

	return (n == 3 ? WSV_OK : WSV_ERROR);
}

int
wsv_ipv6_binary_form(const char* first,
					 const char* last,
					 unsigned char ipv6_addr[16])
{
	unsigned char*	buf;
	unsigned char	tmp[16];
	const char*		colon;
	int				double_colon;
	int				nbytes;
	int				hexadecimal;
	int				xdigits_seen;

	if (first == last)
		return (WSV_ERROR);

	if (*first == ':')
	{
		++first;
		if (first == last || *first != ':')
			return (WSV_ERROR);
		++first;
		buf = tmp;
		colon = first;
		double_colon = 0;
	}
	else
	{
		buf = ipv6_addr;
		colon = 0;
		double_colon = -1;
	}

	nbytes = 16;
	while (nbytes--)
		ipv6_addr[nbytes] = 0;

	nbytes = 0;
	hexadecimal = 0;
	xdigits_seen = 0;
	while (first != last)
	{
		switch (*first)
		{
		case ':':
			colon = first + 1;
			if (xdigits_seen == 0)
			{
				if (double_colon != -1)
					return (WSV_ERROR);
				buf = tmp;
				double_colon = nbytes;
				++first;
				continue;
			}
			*buf++ = (unsigned char)((hexadecimal >> 8) & 0xff);
			*buf++ = (unsigned char)(hexadecimal & 0xff);
			nbytes += 2;
			if (nbytes == 16)
				return (WSV_ERROR);
			hexadecimal = 0;
			xdigits_seen = 0;
			++first;
			break;

		case '.':
			if (colon == 0)
				return (WSV_ERROR);
			else if (nbytes == 12)
				return (wsv_ipv4_binary_form(colon, last, buf));
			else if (double_colon != -1)
			{
				if (wsv_ipv4_binary_form(colon, last, buf) == WSV_OK)
				{
					buf += 4;
					nbytes += 4;
					goto done;
				}
			}
			else
				return (WSV_ERROR);

		default:
			if (*first >= '0' && *first <= '9')
				hexadecimal = (hexadecimal << 4) + (*first - '0');
			else if (*first >= 'a' && *first <= 'z')
				hexadecimal = (hexadecimal << 4) + (*first - 'a' + 10);
			else if (*first >= 'A' && *first <= 'Z')
				hexadecimal = (hexadecimal << 4) + (*first - 'A' + 10);
			else
				return (WSV_ERROR);
			++xdigits_seen;
			if (xdigits_seen == 5)
				return (WSV_ERROR);
			if (hexadecimal > 0xffff)
				return (WSV_ERROR);
			++first;
		}
	}

	*buf++ = (unsigned char)((hexadecimal >> 8) & 0xff);
	*buf++ = (unsigned char)(hexadecimal & 0xff);
	nbytes += 2;

done:
	if (double_colon != -1)
	{
		if (nbytes > 14 || (nbytes % 2) != 0)
			return (WSV_ERROR);
		buf = tmp;
		double_colon = double_colon + (16 - nbytes);
		while (double_colon != 16)
			ipv6_addr[double_colon++] = *buf++;

		return (WSV_OK);
	}

	return (nbytes == 16 ? WSV_OK : WSV_ERROR);
}
