#include "wsv_retval.h"
#include "wsv_configuration.h"
#include "wsv_http_message.h"
#include "wsv_string.h"

int
wsv_http_request_host(struct wsv_http_message_s* m)
{
	char*							host;
	const struct wsv_server_s*		srv;
	int								n_server;
	int								listen_sockfd;

	if (m->headers[HOST][0] == '\0')
		return (WSV_ERROR_400);
	host = wsv_str_trim(m->headers[HOST], ' ');

	srv = ((const struct wsv_configuration_s*)m->configuration)->servers;
	n_server = ((const struct wsv_configuration_s*)m->configuration)->n_server;

	listen_sockfd = ((struct wsv_server_s*)m->server)->listen_sockfd;

	while (n_server--)
	{
		if (srv->listen_sockfd == listen_sockfd &&
			srv->server_name != 0 &&
			wsv_strcmp(srv->server_name, host) == 0)
		{
			m->server = (const void*)srv;
			break;
		}
		++srv;
	}

	return (WSV_OK);
}
