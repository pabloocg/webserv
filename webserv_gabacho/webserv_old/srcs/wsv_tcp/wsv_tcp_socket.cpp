#include <netinet/in.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_internet_protocol.h"
#include "wsv_string.h"
#include "wsv_configuration.h"

static void
wsv_sockaddr_in6_initialize(struct sockaddr_in6* sockaddr,
							unsigned char ipv6_addr[16],
							unsigned char port[2])
{
	int				n;

	wsv_bzero((void*)sockaddr, sizeof(*sockaddr));

	sockaddr->sin6_family = AF_INET6;

	n = 16;
	while (n--)
		sockaddr->sin6_addr.s6_addr[n] = ipv6_addr[n];

	sockaddr->sin6_port = *((in_port_t*)port);
}

static int
wsv_tcp_socket_v4v6(const void* server)
{
	struct wsv_server_s*		srv;
	int							s_listen_sockfd;
	int							s_sockopt;
	struct sockaddr_in6			s_sockaddr;

	srv = (struct wsv_server_s*)server;

	s_listen_sockfd = wsv_socket(AF_INET6, SOCK_STREAM, 0);
	if (s_listen_sockfd < 0)
		return (WSV_ERROR);
	srv->listen_sockfd = s_listen_sockfd;

	s_sockopt = 0;
	if (wsv_setsockopt(s_listen_sockfd,
					   IPPROTO_IPV6, IPV6_V6ONLY,
					   (void*)&s_sockopt,
					   sizeof(s_sockopt)) < 0) /* dual socket -not portable- */
		return (WSV_ERROR);

	if (wsv_fcntl_int(s_listen_sockfd, F_SETFL, O_NONBLOCK) == -1)
		return (WSV_ERROR);

	wsv_sockaddr_in6_initialize(&s_sockaddr, srv->ipv6_addr, srv->port);
	if (wsv_bind(s_listen_sockfd,
				 (struct sockaddr*)&s_sockaddr,
				 sizeof(s_sockaddr)) < 0)
		return (WSV_ERROR);

	if (wsv_listen(s_listen_sockfd, SOMAXCONN) < 0)
		return (WSV_ERROR);

	return (WSV_OK);
}

static void
wsv_server_addr_unique(struct wsv_server_s* srv, int n_server)
{
	struct wsv_server_s*		it;
	int							n;

	while (n_server)
	{
		it = srv + 1;
		n = n_server - 1;
		while (n--)
		{
			if (it->listen_sockfd == -1 &&
				wsv_str_cmp_16((const char*)srv->ipv6_addr,
				(const char*)it->ipv6_addr) &&
				wsv_str_cmp_2((const char*)srv->port,
				(const char*)it->port))
			{
				--n_server;
				it->listen_sockfd = (int)(it - srv);
				it->duplicate = 1;
			}
			++it;
		}
		++srv;
		--n_server;
	}
}

int
wsv_tcp_sockets_v4v6_initialize(void* configuration)
{
	struct wsv_server_s*		srv;
	int							n_server;

	srv = ((const struct wsv_configuration_s*)configuration)->servers;
	n_server = ((const struct wsv_configuration_s*)configuration)->n_server;

	wsv_server_addr_unique(srv, n_server);

	while (n_server--)
	{
		if (srv->listen_sockfd == -1)
		{
			if (wsv_tcp_socket_v4v6((const void*)srv) == WSV_ERROR)
				return (WSV_ERROR);
		}
		else
			srv->listen_sockfd = (srv - srv->listen_sockfd)->listen_sockfd;
		++srv;
	}

	return (WSV_OK);
}
