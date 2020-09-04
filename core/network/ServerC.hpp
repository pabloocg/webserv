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
# define MAX_CLIENTS 200

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
	std::vector<http::ServerConf>		_servers;
	std::map<int, http::Pending_send>	_pending_messages;
	std::map<std::string, std::string>	_mime_types;
	std::map<int, std::string>			_pending_reads;
	std::string							_host_header;
	std::vector<std::string> 			_env;
	bool								_bad_request;

public:

	ServerC(std::vector<http::ServerConf> servers, std::map<std::string, std::string> mime_types, char **env);
	virtual ~ServerC() {};

	void	start();
	void	wait_for_connection();
	void	accept_connection(SA_IN & address, int i);
	void	manage_new_connection(int *server_sckt, SA_IN address, int serv_num);
	void	manage_reads(int &sd);
	void	manage_writes(int &sd);
	bool	valid_req_format(std::string buffer);
	http::ServerConf get_server(void);
	http::ServerConf get_default_server(void);
	class		ServerError: public std::exception
	{
		private:
			std::string		_function;
			std::string		_error;
			const Logger	_log;

		public:
			ServerError(void);
			ServerError(std::string function, std::string error);
			virtual				~ServerError(void) throw() {};
			virtual const char	*what(void) const throw();
	};

};

} // namespace http

#endif
