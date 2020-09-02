#include <stdint.h>

#include "wsv_string.h"

/*
** +) little endian: 0xffff0000
**					 (Least Significant Byte first [LSB])
** +) big endian: 0x0000ffff -> 0xffff
**				  (Most Significant Byte first [MSB])
**				  (leading zeros are ignored)
*/
#define IPV4_MAPPED_ON_IPV6(ip)		((uint32_t*)ip)[0] == 0x0 && \
									((uint32_t*)ip)[1] == 0x0 && \
									((uint32_t*)ip)[2] == 0xffff0000

void
wsv_ipv4_text_form(const unsigned char ipv4_addr[4], char dst[16])
{
	unsigned char		byte;

	byte = ipv4_addr[0];
	dst[2] = (char)('0' + byte % 10);
	byte /= 10;
	dst[1] = (char)('0' + byte % 10);
	dst[0] = (char)('0' + byte / 10);

	dst[3] = '.';

	byte = ipv4_addr[1];
	dst[6] = (char)('0' + byte % 10);
	byte /= 10;
	dst[5] = (char)('0' + byte % 10);
	dst[4] = (char)('0' + byte / 10);

	dst[7] = '.';

	byte = ipv4_addr[2];
	dst[10] = (char)('0' + byte % 10);
	byte /= 10;
	dst[9] = (char)('0' + byte % 10);
	dst[8] = (char)('0' + byte / 10);

	dst[11] = '.';

	byte = ipv4_addr[3];
	dst[14] = (char)('0' + byte % 10);
	byte /= 10;
	dst[13] = (char)('0' + byte % 10);
	dst[12] = (char)('0' + byte / 10);

	dst[15] = '\0';
}

void
wsv_ipv6_text_form(const unsigned char ipv6_addr[16], char dst[40])
{
	static const char	hexadecimal[] = "0123456789abcdef";

	if (IPV4_MAPPED_ON_IPV6(ipv6_addr))
	{
		wsv_str_cpy_8(dst, "::ffff:");
		wsv_ipv4_text_form(ipv6_addr + 12, dst + 7);
	}
	else
	{
		unsigned char	byte;

		byte = ipv6_addr[0];
		dst[0] = hexadecimal[(byte & 0xf0) >> 4];
		dst[1] = hexadecimal[byte & 0xf];
		byte = ipv6_addr[1];
		dst[2] = hexadecimal[(byte & 0xf0) >> 4];
		dst[3] = hexadecimal[byte & 0xf];

		dst[4] = ':';

		byte = ipv6_addr[2];
		dst[5] = hexadecimal[(byte & 0xf0) >> 4];
		dst[6] = hexadecimal[byte & 0xf];
		byte = ipv6_addr[3];
		dst[7] = hexadecimal[(byte & 0xf0) >> 4];
		dst[8] = hexadecimal[byte & 0xf];

		dst[9] = ':';

		byte = ipv6_addr[4];
		dst[10] = hexadecimal[(byte & 0xf0) >> 4];
		dst[11] = hexadecimal[byte & 0xf];
		byte = ipv6_addr[5];
		dst[12] = hexadecimal[(byte & 0xf0) >> 4];
		dst[13] = hexadecimal[byte & 0xf];

		dst[14] = ':';

		byte = ipv6_addr[6];
		dst[15] = hexadecimal[(byte & 0xf0) >> 4];
		dst[16] = hexadecimal[byte & 0xf];
		byte = ipv6_addr[7];
		dst[17] = hexadecimal[(byte & 0xf0) >> 4];
		dst[18] = hexadecimal[byte & 0xf];

		dst[19] = ':';

		byte = ipv6_addr[8];
		dst[20] = hexadecimal[(byte & 0xf0) >> 4];
		dst[21] = hexadecimal[byte & 0xf];
		byte = ipv6_addr[9];
		dst[22] = hexadecimal[(byte & 0xf0) >> 4];
		dst[23] = hexadecimal[byte & 0xf];

		dst[24] = ':';

		byte = ipv6_addr[10];
		dst[25] = hexadecimal[(byte & 0xf0) >> 4];
		dst[26] = hexadecimal[byte & 0xf];
		byte = ipv6_addr[11];
		dst[27] = hexadecimal[(byte & 0xf0) >> 4];
		dst[28] = hexadecimal[byte & 0xf];

		dst[29] = ':';

		byte = ipv6_addr[12];
		dst[30] = hexadecimal[(byte & 0xf0) >> 4];
		dst[31] = hexadecimal[byte & 0xf];
		byte = ipv6_addr[13];
		dst[32] = hexadecimal[(byte & 0xf0) >> 4];
		dst[33] = hexadecimal[byte & 0xf];

		dst[34] = ':';

		byte = ipv6_addr[14];
		dst[35] = hexadecimal[(byte & 0xf0) >> 4];
		dst[36] = hexadecimal[byte & 0xf];
		byte = ipv6_addr[15];
		dst[37] = hexadecimal[(byte & 0xf0) >> 4];
		dst[38] = hexadecimal[byte & 0xf];

		dst[39] = '\0';
	}
}
