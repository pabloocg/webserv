#include <limits.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_configuration.h"
#include "wsv_string.h"
#include "wsv_internet_protocol.h"
#include "wsv_linked_list.h"
#include "wsv_tcp.h"

#define SRV_SIZE	sizeof(struct wsv_server_s)

int
wsv_conf_server_block(struct wsv_configuration_s* conf,
					  struct wsv_conf_data_s* cd)
{
	struct wsv_server_s*	servers;

	if ((cd->key_end - cd->key_begin) != 6 ||
		!wsv_str_cmp_6(cd->key_begin, "server") ||
		cd->value_begin != 0 ||
		conf->n_server == SERVER_SOCKET_MAX)
	{
		return (WSV_ERROR);
	}

	servers = (struct wsv_server_s*)wsv_malloc(SRV_SIZE * (conf->n_server + 1));
	if (servers == 0)
		return (WSV_ERROR);

	/* copy server array */
	wsv_memcpy((void*)servers,
			   (const void*)conf->servers,
			   SRV_SIZE * conf->n_server);

	/* update configuration server */
	free(conf->servers);
	conf->servers = servers;

	servers += conf->n_server;
	conf->srv = servers;

	conf->n_server += 1;

	/* initialize new server member */
	wsv_bzero((void*)servers, SRV_SIZE);
	if (wsv_list_initialize(&(servers->lst_locations)) == WSV_ERROR)
	{
		free((void*)servers);
		return (WSV_ERROR);
	}
	servers->listen_sockfd = -1;

	cd->context = block_server;
	return (WSV_OK);
}

int
wsv_conf_server_directive(struct wsv_server_s* srv,
						  struct wsv_conf_data_s* cd)
{
	if (cd->value_begin == 0)
		return (WSV_ERROR);

	switch (cd->key_end - cd->key_begin)
	{
	case 6:
		if (wsv_str_cmp_6(cd->key_begin, "listen"))
		{
			if (wsv_conf_directive_listen(cd->value_begin,
											  cd->value_end,
											  srv->ipv6_addr,
											  srv->port) == WSV_OK)
			{
				wsv_ipv6_text_form(srv->ipv6_addr, srv->ipv6_addr_str);
				wsv_port_text_form(srv->port, srv->port_str);
				return (WSV_OK);
			}
			else
				return (WSV_ERROR);
		}
	case 10:
		if (wsv_str_cmp_10(cd->key_begin, "error_page"))
		{
			return (wsv_conf_directive_error_page(cd->value_begin,
												  cd->value_end,
												  srv->error_pages));
		}
	case 11:
		if (wsv_str_cmp_11(cd->key_begin, "server_name"))
		{
			return (wsv_conf_directive_string(cd->value_begin,
											  cd->value_end,
											  &srv->server_name));
		}
		break;
	}

	return (WSV_ERROR);
}
