#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_client.h"
#include "wsv_configuration.h"

struct			wsv_multiplexing_s
{
	fd_set		read_set;
	fd_set		write_set;
	fd_set		static_content_set;
	int			max_fd;
};

void
wsv_tcp_multiplexing_read(int fd, void* fd_sets)
{
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->read_set));
	FD_CLR(fd, &(((struct wsv_multiplexing_s*)fd_sets)->write_set));
	if (fd > ((struct wsv_multiplexing_s*)fd_sets)->max_fd)
		((struct wsv_multiplexing_s*)fd_sets)->max_fd = fd;
}

void
wsv_tcp_multiplexing_write(int fd, void* fd_sets)
{
	FD_CLR(fd, &(((struct wsv_multiplexing_s*)fd_sets)->read_set));
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->write_set));
	if (fd > ((struct wsv_multiplexing_s*)fd_sets)->max_fd)
		((struct wsv_multiplexing_s*)fd_sets)->max_fd = fd;
}

void
wsv_tcp_multiplexing_static_content_read(int fd, void* fd_sets)
{
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->read_set));
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->static_content_set));
	if (fd > ((struct wsv_multiplexing_s*)fd_sets)->max_fd)
		((struct wsv_multiplexing_s*)fd_sets)->max_fd = fd;
}

void
wsv_tcp_multiplexing_static_content_write(int fd, void* fd_sets)
{
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->write_set));
	FD_SET(fd, &(((struct wsv_multiplexing_s*)fd_sets)->static_content_set));
	if (fd > ((struct wsv_multiplexing_s*)fd_sets)->max_fd)
		((struct wsv_multiplexing_s*)fd_sets)->max_fd = fd;
}

void
wsv_tcp_multiplexing_clear(int fd, void* fd_sets)
{
	FD_CLR(fd, &(((struct wsv_multiplexing_s*)fd_sets)->read_set));
	FD_CLR(fd, &(((struct wsv_multiplexing_s*)fd_sets)->write_set));
	FD_CLR(fd, &(((struct wsv_multiplexing_s*)fd_sets)->static_content_set));
	if (fd == ((struct wsv_multiplexing_s*)fd_sets)->max_fd)
		((struct wsv_multiplexing_s*)fd_sets)->max_fd -= 1;
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

	struct wsv_multiplexing_s	fd_sets;
	fd_set						to_read_set;
	fd_set						to_write_set;
	int							maxfd;
	int							minfd;
	int							nready;
	int							fd;

	FD_ZERO(&fd_sets.read_set);
	FD_ZERO(&fd_sets.write_set);
	FD_ZERO(&fd_sets.static_content_set);
	fd_sets.max_fd = 0;

	srv = ((const struct wsv_configuration_s*)configuration)->servers;
	n_server = ((const struct wsv_configuration_s*)configuration)->n_server;
	while (n_server--)
	{
		wsv_tcp_multiplexing_read(srv->listen_sockfd, (void*)&fd_sets);
		minfd = srv->listen_sockfd;
		++srv;
	}

	for (;;)
	{
		to_read_set = fd_sets.read_set;
		to_write_set = fd_sets.write_set;
		maxfd = fd_sets.max_fd;

		nready = wsv_select(maxfd + 1, &to_read_set, &to_write_set, 0, 0);
		if (nready < 0)
			return (WSV_ERROR);


		srv = ((const struct wsv_configuration_s*)configuration)->servers;
		n_server = ((const struct wsv_configuration_s*)configuration)->n_server;
		while (n_server--)
		{
			if (srv->duplicate == 0 &&
				FD_ISSET(srv->listen_sockfd, &to_read_set))
			{
				wsv_client_insert(clients,
								  configuration,
								  (const void*)srv,
								  (void*)&fd_sets);
				--nready;
			}
			++srv;
		}

		for (fd = minfd + 1; fd <= maxfd && nready != 0; ++fd)
		{
			if (FD_ISSET(fd, &to_read_set))
			{
				if (FD_ISSET(fd, &(fd_sets.static_content_set)))
					wsv_client_static_content_read(fd);
				else
					wsv_client_receive(fd, clients);
				--nready;
			}
		}

		for (fd = minfd + 1; fd <= maxfd && nready != 0; ++fd)
		{
			if (FD_ISSET(fd, &to_write_set))
			{
				if (FD_ISSET(fd, &(fd_sets.static_content_set)))
					wsv_client_static_content_write(fd);
				else
					wsv_client_send(fd, clients);
				--nready;
			}
		}
	}
}
