#include "webserv.h"
#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_error_log.h"
#include "wsv_tcp.h"
#include "wsv_client.h"
#include "wsv_mime_type.h"
#include "wsv_http_header.h"
#include "wsv_location.h"
#include "wsv_configuration.h"
#include "wsv_string.h"
#include "wsv_linked_list.h"

static struct wsv_http_message_s	*g_clients;
struct wsv_configuration_s			g_configuration;

/*
** NOTES:
** +) close(socket) : causes TIMEWAIT until tcp conection has
**	properly shutdown (if client has not disconnected), could be
**	overridden with socket options (ex: SO_LINGER before shutdown,
**	SO_REUSEADDR at start-up...) but it is preferable for clients
**	to initiate connection close
*/
void
webserv_exit(int status)
{
	struct wsv_server_s*			srv;
	int								n;

	/* ---- configuration buffer ---- */
	free(g_configuration.buf);

	/* ---- configuration servers ---- */
	srv = g_configuration.servers;
	n = g_configuration.n_server;
	while (n--)
	{
		srv->cgi.clear();

		wsv_list_clear(srv->lst_locations, &free);
		wsv_locations_clear(srv->location_buckets,
							srv->location_buckets_size);
		if (srv->duplicate == 0 && srv->listen_sockfd >= 0)
			wsv_close(srv->listen_sockfd);
		++srv;
	}
	free(g_configuration.servers);

	/* ---- clients ---- */
	wsv_clients_terminate(g_clients);

	/* ---- error log ---- */
	wsv_error_log_close();

	delete[] g_clients;

	exit(status);
}

static void
sig_handler(int signum)
{
	if (signum == SIGINT)
		webserv_exit(WSV_EXIT);
	else if (signum == SIGPIPE)
		wsv_error_log("SIGPIPE signal received: ");
	else
		return;
}

static void
webserv_data_initialize(void)
{
	int						n;

	n = FD_SETSIZE;
	while (n--)
		g_clients[n].sockfd = -1;

	wsv_bzero((void*)&g_configuration, sizeof(struct wsv_configuration_s));
}

int
main(void)
{
	int				n;

	g_clients = new wsv_http_message_s[FD_SETSIZE];
	if (g_clients == 0)
		return (WSV_EXIT);

	webserv_data_initialize();

	wsv_error_log_open("error.log");

	if (wsv_signal(SIGINT, &sig_handler) == SIG_ERR)
		webserv_exit(WSV_EXIT_ERR_SIGNAL);
	if (wsv_signal(SIGPIPE, &sig_handler) == SIG_ERR)
		webserv_exit(WSV_EXIT_ERR_SIGNAL);

	if (wsv_configuaration("webserv.conf", &g_configuration) == WSV_ERROR)
		webserv_exit(WSV_EXIT_ERR_CONF);
	if (g_configuration.n_server == 0)
		webserv_exit(WSV_EXIT_ERR_CONF);

	if (wsv_mime_types_initialize() == WSV_ERROR)
		webserv_exit(WSV_EXIT_ERR_MIME);
	if (wsv_http_headers_initialize() == WSV_ERROR)
		webserv_exit(WSV_EXIT_ERR_HTTP_HEADER);

	n = wsv_tcp_sockets_v4v6_initialize((void*)&g_configuration);
	if (n == WSV_ERROR)
		webserv_exit(WSV_EXIT_ERR_SOCKET);

	n = wsv_tcp_multiplexing((void*)g_clients, (const void*)&g_configuration);
	if(n == WSV_ERROR)
		webserv_exit(WSV_EXIT_ERR_SERVER);

	webserv_exit(WSV_EXIT);
}
