#include "wsv_retval.h"
#include "wsv_configuration.h"
#include "wsv_string.h"

int
wsv_conf_process(struct wsv_configuration_s* conf, struct wsv_conf_data_s* cd)
{
	static int		n_block = 0;
	int				n_block_diff;

	n_block_diff = cd->nblock - n_block;
	n_block = cd->nblock;

	switch ((enum g_wsv_context)cd->context)
	{
	case block_none:
		if (n_block_diff != 1)
			return (WSV_ERROR);
		return (wsv_conf_server_block(conf, cd));

	case block_server:
		switch (n_block_diff)
		{
			case -1:
				cd->context = block_none;
				return (WSV_OK);
			case 0:
				return (wsv_conf_server_directive(conf->srv, cd));
			case 1:
				return (wsv_conf_location_block(conf->srv, cd));
		}

	case block_location:
		switch (n_block_diff)
		{
			case -1:
				cd->context = block_server;
				return (WSV_OK);
			case 0:
				return (wsv_conf_location_directive(conf->srv, cd));
			case 1:
				return (wsv_conf_limit_except_block(conf->srv, cd));
		}

	case block_limit_execept:
		switch (n_block_diff)
		{
			case -1:
				cd->context = block_location;
				return (WSV_OK);
			case 0:
				return (wsv_conf_limit_except_directive(conf->srv, cd));
			case 1:
				return (WSV_ERROR);
		}
	}

	return (WSV_ERROR);
}
