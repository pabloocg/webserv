#ifndef WSV_HTTP_HEADER_H
# define WSV_HTTP_HEADER_H

# define HEADER_VALUE_SIZE			512
# define HEADERS_MAX				18
# define HEADERS_BUCKETS_SIZE		41 /* prime number */

# define TRANSFER_ENCODING			0
# define WWW_AUTHENTICATE			1
# define CONTENT_LOCATION			2
# define CONTENT_LANGUAGE			3
# define ACCEPT_LANGUAGE			4
# define ACCEPT_CHARSET				5
# define CONTENT_LENGTH				6
# define LAST_MODIFIED				7
# define AUTHORIZATION				8
# define CONTENT_TYPE				9
# define RETRY_AFTER				10
# define USER_AGENT					11
# define LOCATION					12
# define REFERER					13
# define SERVER						14
# define ALLOW						15
# define DATE						16
# define HOST						17

int		wsv_http_headers_initialize(void);
int		wsv_http_header(const char* header, int* index);

#endif
