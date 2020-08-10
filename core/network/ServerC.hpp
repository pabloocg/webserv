#ifndef SERVERC_HPP
#define SERVERC_HPP

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
# include <fcntl.h>
# include "../config/Logger.hpp"
# include "../utils/Request.hpp"
# include "../utils/utils.hpp"
# include "../config/ServerConf.hpp"

#define TRUE 1
#define FALSE 0

namespace http
{

class ServerC
{

private:

	typedef struct sockaddr_in SA_IN;

	std::vector<int>				_server_socket;
	std::vector<int>				_client_socket;
	int								_max_client;
	std::vector<fd_set>				_master;
	const Logger					_log;
	std::vector<http::ServerConf>	_servers;

public:

	ServerC();
	ServerC(std::vector<http::ServerConf> servers);
	virtual ~ServerC() {};

	void start();
	void wait_for_connection();

};

} // namespace http

#endif
