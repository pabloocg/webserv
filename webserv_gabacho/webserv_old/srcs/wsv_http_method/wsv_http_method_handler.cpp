#include "wsv_retval.h"
#include "wsv_location.h"
#include "wsv_http_message.h"
#include "wsv_configuration.h"
#include "wsv_string.h"

typedef int (*wsv_http_method_t)(struct wsv_http_message_s*, const void*);
typedef int (*wsv_http_method_cgi_t)(struct wsv_http_message_s*, const void*, std::string&);

/* ------ HTTP Methods: branch/jump table ------ */
int		wsv_http_method_get(struct wsv_http_message_s*, const void*);
int		wsv_http_method_head(struct wsv_http_message_s*, const void*);
int		wsv_http_method_put(struct wsv_http_message_s*, const void*);
int		wsv_http_method_delete(struct wsv_http_message_s*, const void*);
int		wsv_http_method_options(struct wsv_http_message_s*, const void*);
int		wsv_http_method_trace(struct wsv_http_message_s*, const void*);

int		wsv_http_method_cgi(struct wsv_http_message_s*, const void*, std::string&);
int		wsv_http_method_cgi_no_body(struct wsv_http_message_s*, const void*, std::string&);
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

	wsv_http_method_cgi_t		wsv_http_methods_cgi[] =
	{
		0,
		&wsv_http_method_cgi_no_body, // get
		&wsv_http_method_cgi_no_body, // head
		&wsv_http_method_cgi, // post
		&wsv_http_method_cgi, //put
		&wsv_http_method_cgi_no_body, // delete
	};

	int								method;
	struct wsv_server_s*			srv;
	const struct wsv_location_s*	location;

	method = m->method;
	srv = (struct wsv_server_s*)m->server;

	if (wsv_location2(srv->location_buckets,
					 srv->location_buckets_size,
					 m->absolute_path,
					 &location) == WSV_OK ||
		wsv_location2(srv->location_buckets,
					 srv->location_buckets_size,
					 m->abs_path_directory,
					 &location) == WSV_OK)
	{

		m->limit_except = location->limit_except;
		if ((location->limit_except & (1 << method)))
		{
			if(srv->cgi[std::string(m->abs_path_extension)] != ""){
				 return (wsv_http_methods_cgi[method](m, (const void*)location,
				 		srv->cgi[m->abs_path_extension]));
			}

			return (wsv_http_methods[method](m, (const void*)location));
		}
	}

	return (WSV_ERROR_405);
}
