#include <stdlib.h>

#include "wsv_retval.h"
#include "wsv_configuration.h"
#include "wsv_string.h"
#include "wsv_location.h"

/*
** NOTES:
** +) We are using PATH_MAX and NAME_MAX from limits.h as reference for
**	the max number of bytes in a pathname or filename. We are more then aware
**	that it doesn't accurately indicated the max values, most os support
**	larger paths. This is one of the many constraint we have set.
*/

int
wsv_configuaration(const char* pathname, struct wsv_configuration_s* conf)
{
	struct wsv_conf_data_s		cd;
	int							retval;
	struct wsv_server_s*		srv;
	int							n;

	wsv_bzero((void*)&cd, sizeof(struct wsv_conf_data_s));

	if (wsv_conf_read(pathname, conf, &cd) == WSV_ERROR)
		return (WSV_ERROR);


	while ((retval = wsv_conf_parse(&cd)) == WSV_RETRY)
	{
		if (wsv_conf_process(conf, &cd) == WSV_ERROR)
			return (WSV_ERROR);
	}

	srv = conf->servers;
	n = conf->n_server;
	if (retval == WSV_OK)
	{
		while (n--)
		{
			retval = wsv_locations_initialize(srv->lst_locations,
											  &srv->location_buckets,
											  &srv->location_buckets_size);
			if (retval == WSV_ERROR)
				break;
			else
			{
				srv->lst_locations = 0;
				++srv;
			}
		}
	}

	return (retval);
}
