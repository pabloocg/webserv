#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_client.h"
#include "wsv_configuration.h"
#include "wsv_tcp.h"

typedef wsv_configuration_s		wsv_configuration_t;

struct							wsv_multiplexing_s
{
	fd_set						read_set;
	fd_set						write_set;
	fd_set						static_content_set;

	fd_set						maxfd_set;
	int							maxfd;

	fd_set						cgi_set;
	int							cgi_active;
	int							cgi_idle;
};

void
wsv_tcp_multiplexing_read(int fd, void* fd_sets)
{
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->read_set));
	FD_CLR(fd, &(((struct wsv_multiplexing_s*)fd_sets)->write_set));

	if (fd > ((struct wsv_multiplexing_s*)fd_sets)->maxfd)
		((struct wsv_multiplexing_s*)fd_sets)->maxfd = fd;
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->maxfd_set));
}

void
wsv_tcp_multiplexing_write(int fd, void* fd_sets)
{
	FD_CLR(fd, &(((struct wsv_multiplexing_s*)fd_sets)->read_set));
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->write_set));

	if (fd > ((struct wsv_multiplexing_s*)fd_sets)->maxfd)
		((struct wsv_multiplexing_s*)fd_sets)->maxfd = fd;
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->maxfd_set));
}

void
wsv_tcp_multiplexing_static_content_read(int fd, void* fd_sets)
{
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->read_set));
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->static_content_set));

	if (fd > ((struct wsv_multiplexing_s*)fd_sets)->maxfd)
		((struct wsv_multiplexing_s*)fd_sets)->maxfd = fd;
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->maxfd_set));
}

void
wsv_tcp_multiplexing_static_content_write(int fd, void* fd_sets)
{
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->write_set));
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->static_content_set));

	if (fd > ((struct wsv_multiplexing_s*)fd_sets)->maxfd)
		((struct wsv_multiplexing_s*)fd_sets)->maxfd = fd;
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->maxfd_set));
}

void
wsv_tcp_multiplexing_clear(int fd, void* fd_sets)
{
	struct wsv_multiplexing_s*	fdsets;

	fdsets = (struct wsv_multiplexing_s*)fd_sets;

	FD_CLR(fd, &(fdsets->read_set));
	FD_CLR(fd, &(fdsets->write_set));
	FD_CLR(fd, &(fdsets->static_content_set));

	FD_CLR(fd, &(fdsets->maxfd_set));

	if (fdsets->maxfd == fd)
	{
		while (fd--)
		{
			if (FD_ISSET(fd, &(fdsets->maxfd_set)))
			{
				fdsets->maxfd = fd;
				break;
			}
		}
	}
}

void
wsv_tcp_multiplexing_cgi_insert(int fd, void* fd_sets)
{
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->cgi_set));
	((struct wsv_multiplexing_s*)fd_sets)->cgi_idle += 1;
}

void
wsv_tcp_multiplexing_cgi_clear(void* fd_sets)
{
	((struct wsv_multiplexing_s*)fd_sets)->cgi_active -= 1;
}


/*
** NOTES:
** +) dependency: new file descriptor is the lowest-numbered fd available,
**	meaning that every client c_sockfd is greater then s_listen_sockfd
*/
int
wsv_tcp_multiplexing(void* clients, const void* configuration)
{
	const struct wsv_server_s*	srv;
	int							n_server;
	int							n_client;
	int							max_clients;

	struct wsv_multiplexing_s	fd_sets;

	int							maxfd;
	int							minfd;
	fd_set						to_read_set;
	fd_set						to_write_set;
	struct timeval				timeout;
	int							nready;
	int							fd;

	srv = ((const wsv_configuration_t*)configuration)->servers;
	n_server = ((const wsv_configuration_t*)configuration)->n_server;
	n_client = 0;
	max_clients = (CLIENT_SOCKET_MAX >> 1) - n_server;
	while (n_server--)
	{
		maxfd = srv->listen_sockfd;
		minfd = srv->listen_sockfd + 1;
		++srv;
	}

	FD_ZERO(&fd_sets.read_set);
	FD_ZERO(&fd_sets.write_set);
	FD_ZERO(&fd_sets.static_content_set);
	FD_ZERO(&fd_sets.maxfd_set);
	fd_sets.maxfd = maxfd;
	FD_ZERO(&fd_sets.cgi_set);
	fd_sets.cgi_active = 0;
	fd_sets.cgi_idle = 0;

	for (;;)
	{
		to_read_set = fd_sets.read_set;
		to_write_set = fd_sets.write_set;
		maxfd = fd_sets.maxfd;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		if (n_client < max_clients)
		{
			srv = ((const wsv_configuration_t*)configuration)->servers;
			n_server = ((const wsv_configuration_t*)configuration)->n_server;
			while (n_server--)
			{
				FD_SET(srv->listen_sockfd, &to_read_set);
				++srv;
			}
		}

		nready = wsv_select(maxfd + 1,
							&to_read_set,
							&to_write_set,
							0,
							&timeout);
		if (nready < 0)
			return (WSV_ERROR);

		if (nready == 0)
		{
			for (fd = minfd; fd <= FD_MAX; ++fd)
			{
				if (FD_ISSET(fd, &fd_sets.read_set) ||
					FD_ISSET(fd, &fd_sets.write_set))
				{
					if (!FD_ISSET(fd, &(fd_sets.static_content_set)))
						n_client = wsv_client_timeout(fd, clients, n_client);
				}
			}
			continue;
		}

		srv = ((const struct wsv_configuration_s*)configuration)->servers;
		n_server = ((const struct wsv_configuration_s*)configuration)->n_server;
		while (n_server--)
		{
			if (srv->duplicate == 0 &&
				FD_ISSET(srv->listen_sockfd, &to_read_set))
			{
				n_client = wsv_client_insert(clients,
											 n_client,
											 configuration,
											 (const void*)srv,
											 (void*)&fd_sets);
				--nready;
			}
			++srv;
		}

		for (fd = minfd; fd <= maxfd && nready != 0; ++fd)
		{
			if (FD_ISSET(fd, &to_read_set))
			{
				if (FD_ISSET(fd, &(fd_sets.static_content_set)))
					n_client = wsv_client_static_content_read(fd, n_client);
				else
					n_client = wsv_client_read(fd, clients, n_client);
				--nready;
			}
		}

		for (fd = minfd; fd <= maxfd && nready != 0; ++fd)
		{
			if (FD_ISSET(fd, &to_write_set))
			{
				if (FD_ISSET(fd, &(fd_sets.static_content_set)))
					n_client = wsv_client_static_content_write(fd, n_client);
				else
					n_client = wsv_client_write(fd, clients, n_client);
				--nready;
			}
		}

		for (fd = minfd; fd_sets.cgi_active < 1 && fd_sets.cgi_idle != 0; ++fd)
		{
			if (FD_ISSET(fd, &fd_sets.cgi_set))
			{
				FD_CLR(fd, &fd_sets.cgi_set);
				fd_sets.cgi_active += 1;
				fd_sets.cgi_idle -= 1;

					n_client = wsv_client_cgi(fd, clients, n_client);
			}
		}
	}
}
