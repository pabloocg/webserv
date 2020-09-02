#include <iostream>

#include <netinet/in.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_client.h"
#include "wsv_http_message.h"
#include "wsv_string.h"
#include "wsv_tcp.h"
#include "wsv_configuration.h"
#include "wsv_internet_protocol.h"
#include "wsv_http_cgi.h"

/*
** NOTES:
** +) used to idenitfy the corresponding client to a static
**	content file descriptoor (fd)
*/
struct wsv_http_message_s*		g_clients_static_content[FD_MAX];

int		wsv_http_method_handler(struct wsv_http_message_s* m);

/*
** =======================
** = Client(s) modifiers =
** =======================
*/
int
wsv_client_timeout(int c_sockfd, void* clients, int n_client)
{
	struct wsv_http_message_s*	m;

	m = (struct wsv_http_message_s*)(clients) + c_sockfd;

	m->cgi_headers.clear();
	m->cgi_string.clear();

	if (m->ressource_fd >= 0)
	{
		wsv_tcp_multiplexing_clear(m->ressource_fd, m->fd_sets);
		wsv_close(m->ressource_fd); /* insert error check */
	}
	if (m->cgi_fd >= 0)
	{
		wsv_tcp_multiplexing_clear(m->cgi_fd, m->fd_sets);
		wsv_close(m->cgi_fd); /* insert error check */
	}
	if (m->tmp_fd >= 0)
	{
		wsv_tcp_multiplexing_clear(m->tmp_fd, m->fd_sets);
		wsv_close(m->tmp_fd); /* insert error check */
	}

	wsv_tcp_multiplexing_clear(m->sockfd, m->fd_sets);
	wsv_close(m->sockfd); /* insert error check */
	m->sockfd = -1;

#ifdef WSV_LOG
	std::cout << "\n+++++++++++++++++++++++++++++\n"
			  << "timeout client ["
			  <<  m->client_ipv6_addr_str
			  << "]:"
			  <<  m->client_port_str
			  << "\n+++++++++++++++++++++++++++++"
			  << std::endl;
#endif

	return (n_client - 1);
}

static void
wsv_client_remove(struct wsv_http_message_s* m)
{
	wsv_tcp_multiplexing_clear(m->sockfd, m->fd_sets);
	wsv_close(m->sockfd); /* insert error check */
	m->sockfd = -1;

#ifdef WSV_LOG
	std::cout << "\n+++++++++++++++++++++++++++++\n"
			  << "closed client ["
			  <<  m->client_ipv6_addr_str
			  << "]:"
			  <<  m->client_port_str
			  << "\n+++++++++++++++++++++++++++++"
			  << std::endl;
#endif

}

void
wsv_clients_terminate(void* clients)
{
	struct wsv_http_message_s*	m;
	int							n;

	n = FD_MAX;
	while (n--)
	{
		if (((struct wsv_http_message_s*)clients)[n].sockfd != -1)
		{
			m = (struct wsv_http_message_s*)(clients) + n;
			wsv_client_remove(m);
		}
	}
}

int
wsv_client_insert(void* clients,
				  int n_client,
				  const void* configuration,
				  const void* server,
				  void* fd_sets)
{
	int							c_sockfd;
	socklen_t					c_addrlen;
	struct sockaddr_in6			c_addr;
	struct wsv_http_message_s*	m;

	/* accept socket connection */
	c_addrlen = sizeof(c_addr);
	c_sockfd = wsv_accept(((struct wsv_server_s*)server)->listen_sockfd,
						  (struct sockaddr*)&c_addr,
						  &c_addrlen);
	if (c_sockfd < 0 || c_addrlen > sizeof(c_addr))
		return (n_client);

	if (n_client == CLIENT_SOCKET_MAX)
	{
		wsv_close(c_sockfd); /* insert error check */
		return (n_client);
	}

	/* initialize client */
	m = (struct wsv_http_message_s*)(clients) + c_sockfd;
	m->sockfd = c_sockfd;
	m->server = server;
	m->configuration = configuration;
	m->fd_sets = fd_sets;
	wsv_ipv6_text_form(c_addr.sin6_addr.s6_addr, m->client_ipv6_addr_str);
	wsv_port_text_form((unsigned char*)&c_addr.sin6_port, m->client_port_str);

	/* initialize client http message */
	wsv_http_message_initialize(m);

	/* receive http request */
	wsv_tcp_multiplexing_read(c_sockfd, fd_sets);

#ifdef WSV_LOG
	std::cout << "\n+++++++++++++++++++++++++++++\n"
			  << "accepted client ["
			  <<  m->client_ipv6_addr_str
			  << "]:"
			  <<  m->client_port_str
			  << "\n+++++++++++++++++++++++++++++"
			  << std::endl;
#endif

	return (n_client + 1);
}

/*
** ========================
** = I/O [initialisation] =
** ========================
*/
void
wsv_client_static_content_read_intialize(int fd, void* client)
{
	struct wsv_http_message_s*	m;

	m = (struct wsv_http_message_s*)client;

	g_clients_static_content[fd] = m;
	wsv_tcp_multiplexing_clear(m->sockfd, m->fd_sets);
	wsv_tcp_multiplexing_static_content_read(fd, m->fd_sets);
}

void
wsv_client_static_content_write_intialize(int fd, void* client)
{
	struct wsv_http_message_s*	m;

	m = (struct wsv_http_message_s*)client;

	g_clients_static_content[fd] = m;
	wsv_tcp_multiplexing_clear(m->sockfd, m->fd_sets);
	wsv_tcp_multiplexing_static_content_write(fd, m->fd_sets);
}

void
wsv_client_read_initialize(int fd, void* client)
{
	struct wsv_http_message_s*	m;

	m = (struct wsv_http_message_s*)client;

	wsv_tcp_multiplexing_clear(fd, m->fd_sets);
	wsv_tcp_multiplexing_read(m->sockfd, m->fd_sets);
}

void
wsv_client_write_initialize(int fd, void* client)
{
	struct wsv_http_message_s*	m;

	m = (struct wsv_http_message_s*)client;

	wsv_tcp_multiplexing_clear(fd, m->fd_sets);
	wsv_tcp_multiplexing_write(m->sockfd, m->fd_sets);
}

/*
** ================
** = Input/Output =
** ================
*/
int
wsv_client_read(int c_sockfd, void* clients, int n_client)
{
	struct wsv_http_message_s*	m;
	int							retval;

	m = (struct wsv_http_message_s*)(clients) + c_sockfd;

	retval = m->read_request_handler(c_sockfd, m);
	if (retval == WSV_OK)
		retval = wsv_http_method_handler(m);

	switch (retval)
	{
	case WSV_RETRY:
		break;

	case WSV_ERROR:
		wsv_client_remove(m);
		--n_client;
		break;

	default:
		m->status_code = retval;
		wsv_http_response(m);
		wsv_tcp_multiplexing_write(c_sockfd, m->fd_sets);
	}

	return (n_client);
}

int
wsv_client_write(int c_sockfd, void* clients, int n_client)
{
	struct wsv_http_message_s*	m;
	int							retval;

	m = (struct wsv_http_message_s*)(clients) + c_sockfd;

	retval = m->write_response_handler(c_sockfd, m);
	switch (retval)
	{
	case WSV_OK:
		wsv_tcp_multiplexing_read(c_sockfd, m->fd_sets);
		wsv_http_message_initialize(m);
		break;

	case WSV_ERROR:
		wsv_client_remove(m);
		--n_client;
	}

	return (n_client);
}

int
wsv_client_static_content_read(int fd, int n_client)
{
	struct wsv_http_message_s*	m;
	int							retval;

	m = g_clients_static_content[fd];

	retval = m->read_static_content_handler(fd, m);
	switch (retval)
	{
	case WSV_OK:
		wsv_tcp_multiplexing_clear(fd, m->fd_sets);
		wsv_tcp_multiplexing_read(m->sockfd, m->fd_sets);
		wsv_http_message_initialize(m);
		break;

	case WSV_RETRY:
		break;

	case WSV_ERROR:
		wsv_tcp_multiplexing_clear(fd, m->fd_sets);
		wsv_client_remove(m);
		--n_client;
		break;

	default:
		if (WSV_IS_HTTP_ERROR(retval))
		{
			m->status_code = retval;
			wsv_http_response(m);
		}
		else
		{
			m->state_response = 0x2;
		}
		wsv_tcp_multiplexing_clear(fd, m->fd_sets);
		wsv_tcp_multiplexing_write(m->sockfd, m->fd_sets);
		break;
	}

	return (n_client);
}

int
wsv_client_static_content_write(int fd, int n_client)
{
	struct wsv_http_message_s*	m;
	int							retval;

	m = g_clients_static_content[fd];

	retval = m->write_static_content_handler(fd, m);
	if (retval != WSV_RETRY)
	{
		m->status_code = retval;
		wsv_http_response(m);
		wsv_tcp_multiplexing_clear(fd, m->fd_sets);
		wsv_tcp_multiplexing_write(m->sockfd, m->fd_sets);
	}

	return (n_client);
}

int
wsv_client_cgi(int c_sockfd, void* clients, int n_client)
{
	struct wsv_http_message_s*	m;
	int							retval;

	m = (struct wsv_http_message_s*)(clients) + c_sockfd;

	retval = wsv_http_cgi((void*)m);
	if (retval != WSV_RETRY)
	{
		m->status_code = retval;
		wsv_http_response(m);
		wsv_tcp_multiplexing_write(m->sockfd, m->fd_sets);
	}

	return (n_client);
}
