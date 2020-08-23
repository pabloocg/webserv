#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <errno.h>
# include <iostream>
# include <vector>
# include <fstream>
# include <sstream>
# include <string>
# include <algorithm>
# include <map>
# include "../config/ServerConf.hpp"

namespace http
{
	class Request
	{
	private:

	//Request variables
	int type;
	std::string file_req;
	std::string file_type;
	std::string request;
	std::string http_version;
	std::string _auth;
	std::string _realm;
	std::string _auth_file_path;
	http::Routes location;
	http::ServerConf	_server;
	bool	_www_auth_required;
	std::map<int,std::string>	_error_mgs;

	//Response variables
	int status;
	std::string message_status;
	std::string resp_body;

	char *build_get(int *size, std::map<std::string, std::string> mime_types);
	std::string get_content_type(std::string file_type, std::map<std::string, std::string> mime_types);
	bool needs_auth(http::Routes  routes);
	bool validate_password(std::string auth);
	void read_file_requested(void);
	std::map<int, std::string> create_map()
	{
		std::map<int, std::string> m;

		m[200] = "OK";
		m[401] = "Unauthorized";
		m[403] = "Forbidden";
		m[404] = "Not Found";
		m[405] = "Not Allowed";
		return (m);
	};

	public:
		Request(std::string req, http::ServerConf server);

		char *build_response(int *size, std::map<std::string, std::string> mime_types);
		void save_header(std::string header);
	};
} // namespace http

#endif