#include "wsv_http_header.h"
#include "wsv_retval.h"
#include "wsv_hash.h"

static const int				headers_index[HEADERS_MAX] =
{
	TRANSFER_ENCODING,
	WWW_AUTHENTICATE,
	CONTENT_LOCATION,
	CONTENT_LANGUAGE,
	ACCEPT_LANGUAGE,
	ACCEPT_CHARSET,
	CONTENT_LENGTH,
	LAST_MODIFIED,
	AUTHORIZATION,
	CONTENT_TYPE,
	RETRY_AFTER,
	USER_AGENT,
	LOCATION,
	REFERER,
	SERVER,
	ALLOW,
	DATE,
	HOST
};

static struct wsv_pair_s		g_wsv_headers[HEADERS_MAX] =
{
	{ (const void*)"transfer-encoding",	(const void*)(headers_index + 0) },
	{ (const void*)"www-authenticate",	(const void*)(headers_index + 1) },
	{ (const void*)"content-location",	(const void*)(headers_index + 2) },
	{ (const void*)"content-language",	(const void*)(headers_index + 3) },
	{ (const void*)"accept-language",	(const void*)(headers_index + 4) },
	{ (const void*)"accept-charset",	(const void*)(headers_index + 5) },
	{ (const void*)"content-length",	(const void*)(headers_index + 6) },
	{ (const void*)"last-Modified",		(const void*)(headers_index + 7) },
	{ (const void*)"authorization",		(const void*)(headers_index + 8) },
	{ (const void*)"content-type",		(const void*)(headers_index + 9) },
	{ (const void*)"retry-after",		(const void*)(headers_index + 10) },
	{ (const void*)"user-agent",		(const void*)(headers_index + 11) },
	{ (const void*)"location",			(const void*)(headers_index + 12) },
	{ (const void*)"referer",			(const void*)(headers_index + 13) },
	{ (const void*)"server",			(const void*)(headers_index + 14) },
	{ (const void*)"allow",				(const void*)(headers_index + 15) },
	{ (const void*)"date",				(const void*)(headers_index + 16) },
	{ (const void*)"host",				(const void*)(headers_index + 17) }
};

static struct wsv_pair_s		g_wsv_headers_buckets[HEADERS_BUCKETS_SIZE];

int
wsv_http_headers_initialize(void)
{
	return (wsv_hash_str_initialize(g_wsv_headers_buckets,
									HEADERS_BUCKETS_SIZE,
									g_wsv_headers,
									HEADERS_MAX));
}

int
wsv_http_header(const char* header, int* index)
{
	const int*		value;

	if (wsv_hash_str_find(g_wsv_headers_buckets,
						  HEADERS_BUCKETS_SIZE,
						  (const void*)header,
						  (const void**)&value) == WSV_OK)
	{
		*index = *value;
		return (WSV_OK);
	}
	else
		return (WSV_ERROR);
}
