#ifndef WEBSERV_H
# define WEBSERV_H

# define	WSV_EXIT					0
# define	WSV_EXIT_ERR_LOG			1
# define	WSV_EXIT_ERR_SIGNAL			2
# define	WSV_EXIT_ERR_CONF			3
# define	WSV_EXIT_ERR_MIME			4
# define	WSV_EXIT_ERR_HTTP_HEADER	5
# define	WSV_EXIT_ERR_SOCKET			6
# define	WSV_EXIT_ERR_SERVER			7

void	webserv_exit(int status);

#endif
