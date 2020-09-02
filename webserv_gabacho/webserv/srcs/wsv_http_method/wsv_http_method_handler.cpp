#include "wsv_retval.h"
#include "wsv_location.h"
#include "wsv_http_message.h"
#include "wsv_configuration.h"
#include "wsv_string.h"
#include "wsv_http_cgi.h"
#include "wsv_http_method.h"

typedef int (*wsv_http_method_t)(struct wsv_http_message_s*, const void*);

/* ------ HTTP Methods: branch/jump table ------ */
int		wsv_http_method_get(struct wsv_http_message_s*, const void*);
int		wsv_http_method_head(struct wsv_http_message_s*, const void*);
int		wsv_http_method_put(struct wsv_http_message_s*, const void*);
int		wsv_http_method_delete(struct wsv_http_message_s*, const void*);
int		wsv_http_method_options(struct wsv_http_message_s*, const void*);
int		wsv_http_method_trace(struct wsv_http_message_s*, const void*);
/* --------------------------------------------- */

int
wsv_http_method_handler(struct wsv_http_message_s* m)
{
	wsv_http_method_t			wsv_http_methods[] =
	{
		0, /* empty cell */
		&wsv_http_method_get,		/* GET */
		&wsv_http_method_head,		/* HEAD */
		&wsv_http_method_put,		/* POST */
		&wsv_http_method_put,		/* PUT */
		&wsv_http_method_delete,	/* DELETE */
		0,							/* CONNECT */
		&wsv_http_method_options,	/* OPTIONS */
		&wsv_http_method_trace		/* TRACE */
	};
	int								method;
	struct wsv_server_s*			srv;
	const struct wsv_location_s*	location;

	method = m->method;
	srv = (struct wsv_server_s*)m->server;

	if (wsv_location(srv->location_buckets,
					 srv->location_buckets_size,
					 m->absolute_path,
					 &location) == WSV_OK ||
		wsv_location(srv->location_buckets,
					 srv->location_buckets_size,
					 m->abs_path_directory,
					 &location) == WSV_OK)
	{
		m->limit_except = location->limit_except;
		if ((location->limit_except & (1 << method)))
		{
			if (location->cgi_bin != 0)
			{
				if (location->cgi_pass == 0 ||
					wsv_strcmp(location->cgi_pass, m->abs_path_extension) == 0)
				{
					if (m->method == HTTP_METHOD_GET)
						return (wsv_http_cgi_get((void*)m, location));
					if (m->method == HTTP_METHOD_POST)
						return (wsv_http_cgi_post((void*)m, location));
				}
			}

			return (wsv_http_methods[method](m, (const void*)location));
		}
	}

	return (WSV_ERROR_405);
}
