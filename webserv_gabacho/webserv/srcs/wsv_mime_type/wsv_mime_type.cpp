#include "wsv_hash.h"
#include "wsv_retval.h"

#define MIME_TYPES_SIZE		15
#define MIME_BUCKETS_SIZE	31 /* prime number */

static const char*			g_mime_type_default = "application/octet-stream";

static struct wsv_pair_s	g_wsv_mime_types[MIME_TYPES_SIZE] =
{
	{ (const void*)"html",		(const void*)"text/html" },
	{ (const void*)"htm",		(const void*)"text/html" },
	{ (const void*)"shtml",		(const void*)"text/html" },
	{ (const void*)"css",		(const void*)"text/css" },
	{ (const void*)"gif",		(const void*)"image/gif" },
	{ (const void*)"jpeg",		(const void*)"image/jpeg" },
	{ (const void*)"jpg",		(const void*)"image/jpeg" },
	{ (const void*)"js",		(const void*)"application/javascript" },
	{ (const void*)"txt",		(const void*)"text/plain" },
	{ (const void*)"png",		(const void*)"image/png" },
	{ (const void*)"c",			(const void*)"text/x-c" },
	{ (const void*)"cpp",		(const void*)"text/x-c" },
	{ (const void*)"h",			(const void*)"text/x-h" },
	{ (const void*)"mp4" ,		(const void*)"video/mp4" },
	{ (const void*)"",			(const void*)"application/octet-stream" }
};

static struct wsv_pair_s	g_wsv_mime_buckets[MIME_BUCKETS_SIZE];

int
wsv_mime_types_initialize(void)
{
	return (wsv_hash_str_initialize(g_wsv_mime_buckets,
									MIME_BUCKETS_SIZE,
									g_wsv_mime_types,
									MIME_TYPES_SIZE));
}

void
wsv_mime_type(const char* extension, const char** mime_type)
{
	if (wsv_hash_str_find(g_wsv_mime_buckets,
						  MIME_BUCKETS_SIZE,
						  (const void*)(extension + 1),
						  (const void**)mime_type) == WSV_ERROR)
	{
		*mime_type = g_mime_type_default;
	}
}
