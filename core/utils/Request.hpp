#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <dirent.h>
# include <sys/stat.h>
# include <time.h>
# include <errno.h>
# include <iostream>
# include <vector>
# include <fstream>
# include <sstream>
# include <string>
# include <algorithm>
# include <map>
#include <fcntl.h>
# include "../config/ServerConf.hpp"
#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define SIDE_OUT 0
#define SIDE_IN 1

namespace http
{
	class Request
	{
	private:
	bool	is_autoindex;
	//Request variables
	int type;
	std::string file_req;
	std::string file_bef_req;
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
	std::vector<std::string> _allow;
	std::string				_client_info;
	std::vector<std::string> _env;
	std::string					_req_content_type;
	std::string					_req_content_length;
	std::string					_req_URI;
	std::string					_query_string;
	std::string					_path_info;
	std::string					_script_name;
	std::string					_transf_encoding;
	std::string					_request_body;
	std::string					_CGI_response;
	std::string					_language_header;
	std::vector<std::string>	_CGI_headers;
	std::vector<std::string>	_languages_accepted;
	bool						_isCGI;

	//Response variables
	int status;
	std::string message_status;
	std::string resp_body;

	void	build_get(void);
	void	build_post(void);
	void	build_put(void);
	void	build_delete(void);
	void	build_options(void);
	char	*getResponse(int *size, std::map<std::string, std::string> mime_types);
	std::string get_content_type(std::string file_type, std::map<std::string, std::string> mime_types);
	bool needs_auth(http::Routes  routes);
	bool validate_password(std::string auth);
	void set_status(void);
	void get_allowed_methods(void);
	void add_basic_env_vars(void);
	void startCGI(void);
	void save_request_body(void);
	void decode_CGI_response(void);
	void decode_chunked(void);
	void get_languages_vector(void);
	std::map<int, std::string> create_map()
	{
		std::map<int, std::string> m;

		m[200] = "OK";
		m[201] = "Created";
		m[202] = "Accepted";
		m[204] = "No Content";
		m[400] = "Bad Request";
		m[401] = "Unauthorized";
		m[403] = "Forbidden";
		m[404] = "Not Found";
		m[405] = "Not Allowed";
		return (m);
	};

	public:
		Request(std::string req, http::ServerConf server, bool bad_request, std::vector<std::string> env);

		char *build_response(int *size, std::map<std::string, std::string> mime_types);
		void save_header(std::string header);
	};
} // namespace http

#endif