#ifndef WSV_TCP_H
# define WSV_TCP_H

# define LOG_MAX			4 /* stdin, stdout, stderr and error_log */
# define SERVER_SOCKET_MAX	8
# define CLIENT_SOCKET_MAX	128
# define FD_MAX				(LOG_MAX + \
							 SERVER_SOCKET_MAX + \
							 (CLIENT_SOCKET_MAX * 3))

int		wsv_tcp_sockets_v4v6_initialize(void* configuration);
int		wsv_tcp_multiplexing(void* clients, const void* configuration);

void	wsv_tcp_multiplexing_read(int fd, void* fd_sets);
void	wsv_tcp_multiplexing_write(int fd, void* fd_sets);

void	wsv_tcp_multiplexing_static_content_read(int fd, void* fd_sets);
void	wsv_tcp_multiplexing_static_content_write(int fd, void* fd_sets);

void	wsv_tcp_multiplexing_clear(int fd, void* fd_sets);

void	wsv_tcp_multiplexing_cgi_insert(int fd, void* fd_sets);
void	wsv_tcp_multiplexing_cgi_clear(void* fd_sets);

#endif
