#ifndef WSV_INTERNET_PROTOCOL_H
# define WSV_INTERNET_PROTOCOL_H

# ifdef WSV_BIG_EDIAN
#  define FT_HTONS(n)		n
# else
/*
** ==============
** = FT_HTON... =
** ==============
** +) Convert from little endian to network byte order/big endian
**	(from least significant byte [lsb] to most significant bytes [msb] first)
*/
#  define FT_HTONS(n)		(((n & 0xff) << 8) | ((n & 0xff00) >> 8))
# endif

void	wsv_ipv4_text_form(const unsigned char ipv4_addr[4], char dst[16]);
void	wsv_ipv6_text_form(const unsigned char ipv6_addr[16], char dst[40]);

int		wsv_ipv4_binary_form(const char* first,
							 const char* last,
							 unsigned char ipv4_addr[4]);
int		wsv_ipv6_binary_form(const char* first,
							 const char* last,
							 unsigned char ipv6_addr[16]);

void	wsv_port_text_form(const unsigned char port[2], char dst[6]);

int		wsv_port_binary_form(const char* first,
							 const char* last,
							 unsigned char port[2]);

#endif
