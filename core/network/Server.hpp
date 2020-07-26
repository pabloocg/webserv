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
# include "../utils/utils.hpp"
# include "../config/Logger.hpp"

#define TRUE 1
#define FALSE 0
#define PORT 8080
#define ACCESS_LOG_PATH "tmp/logs/access.log"
#define ERROR_LOG_PATH "tmp/logs/error.log"
#define ACCESS_LOG "access.log"
#define ERROR_LOG "error.log"

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
	fd_set readfds;
	const Logger	_log;

public:
	Server();
	//~Server();

	void start();
	void wait_for_connection();
};

} // namespace http

#endif
