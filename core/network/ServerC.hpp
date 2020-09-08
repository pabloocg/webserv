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
# include <queue>
# include "../config/Logger.hpp"
# include "../request/Request.hpp"
# include "../utils/utils.hpp"
# include "../config/ServerConf.hpp"
# include "../client/Client.hpp"

#define TRUE 1
#define FALSE 0
#define MAX_CLIENTS 200
#define MAX_TMP_CLIENTS 20

namespace http
{

class ServerC
{

private:
	ServerC(void);

	typedef struct sockaddr_in SA_IN;

	int									_status_code;
	fd_set								_master_read;
	fd_set								_master_write;
	std::string							_host_header;
	std::vector<int>					_server_socket;
	std::vector<http::Client>			_clients;
	std::queue<http::Client>			_tmp_clients;
	std::vector<http::ServerConf>		_servers;
	std::vector<std::string> 			_env;
	std::map<std::string, std::string>	_mime_types;

public:

	ServerC(std::vector<http::ServerConf> servers, std::map<std::string, std::string> mime_types, char **env);
	virtual ~ServerC() {};

	void	start(void);
	void	wait_for_connection(void);

	void	accept_connection(http::ServerConf &server, int &server_socket);
	void	reject_connection(http::ServerConf &server, int &server_socket);
	void	read_request(char *buf, std::vector<http::Client>::iterator &client, int valread);
	void	add_client(int &new_socket);
	void	remove_client(std::vector<http::Client>::iterator &client);
	void	remove_tmp_client(http::Client  &client);

	bool	valid_req_format(std::string buffer);
	void	send_response(std::string &request, std::vector<http::Client>::iterator &client);

	http::ServerConf get_server(void);
	http::ServerConf get_default_server(void);

	class		ServerError: public std::exception
	{
		private:
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
