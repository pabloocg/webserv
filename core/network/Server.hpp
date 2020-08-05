#ifndef SERVER_HPP
#define SERVER_HPP

# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <errno.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <iostream>
# include <string>
# include "../config/Logger.hpp"
# include "../utils/Request.hpp"
# include "../utils/utils.hpp"
# include "../config/ServerConf.hpp"

#define TRUE 1
#define FALSE 0

namespace http
{

class Server
{
private:
	typedef struct sockaddr_in SA_IN;

	int server_socket;
	int *client_socket;
	int max_client;
	//char *buffer;
	SA_IN address;
	fd_set master;
	const Logger	_log;
	// Each server configuration
	//std::vector<ServerConf>	servers;

public:
	Server();
	//~Server();

	void start();
	void wait_for_connection();
};

} // namespace http

#endif
