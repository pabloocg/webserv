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
# include "../utils/Pending_send.hpp"

#define TRUE 1
#define FALSE 0

namespace http
{

class ServerC
{

private:
	ServerC();

	typedef struct sockaddr_in SA_IN;

	std::vector<int>					_server_socket;
	std::vector<int>					_client_socket;
	int									_max_client;
	fd_set								_master_read;
	fd_set								_master_write;
	const Logger						_log;
	std::vector<http::ServerConf>		_servers;
	std::map<int, http::Pending_send>	_pending_messages;
	std::map<std::string, std::string>	_mime_types;

public:

	ServerC(std::vector<http::ServerConf> servers, std::map<std::string, std::string> mime_types);
	virtual ~ServerC() {};

	void	start();
	void	wait_for_connection();
	void	manage_new_connection(int *server_sckt, SA_IN address);
	void	manage_reads(int *sd);
	void	manage_writes(int *sd);

};

} // namespace http

#endif
