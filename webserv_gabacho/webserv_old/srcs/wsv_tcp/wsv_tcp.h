#ifndef WSV_TCP_H
# define WSV_TCP_H


int		wsv_tcp_sockets_v4v6_initialize(void* configuration);
int		wsv_tcp_multiplexing(void* clients, const void* configuration);

void	wsv_tcp_multiplexing_read(int fd, void* fd_sets);
void	wsv_tcp_multiplexing_write(int fd, void* fd_sets);

void	wsv_tcp_multiplexing_static_content_read(int fd, void* fd_sets);
void	wsv_tcp_multiplexing_static_content_write(int fd, void* fd_sets);

void	wsv_tcp_multiplexing_clear(int fd, void* fd_sets);

#endif
