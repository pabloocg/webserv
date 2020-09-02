#ifndef WSV_CLIENT_H
# define WSV_CLIENT_H

# include "wsv_http_message.h"

void	wsv_clients_terminate(void* clients);

void	wsv_client_insert(void* clients,
						  const void* configuration,
						  const void* server,
						  void* fd_sets);

void	wsv_client_static_content_read_intialize(int fd, void* client);
void	wsv_client_static_content_write_intialize(int fd, void* client);

void	wsv_client_receive_initialize(int fd, void* client);
void	wsv_client_send_initialize(int fd, void* client);

void	wsv_client_receive(int c_sockfd, void* clients);
void	wsv_client_send(int c_sockfd, void* clients);

void	wsv_client_static_content_read(int fd);
void	wsv_client_static_content_write(int fd);

#endif
