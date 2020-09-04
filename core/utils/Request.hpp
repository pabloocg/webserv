#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <dirent.h>
# include <sys/stat.h>
# include <time.h>
# include <signal.h>
# include <errno.h>
# include <iostream>
# include <vector>
# include <fstream>
# include <sstream>
# include <string>
# include <algorithm>
# include <map>
# include <fcntl.h>
# include "../config/ServerConf.hpp"
# define STDIN 0
# define STDOUT 1
# define STDERR 2

# define SIDE_OUT 0
# define SIDE_IN 1

# define BUFFER_SIZE 65536

namespace http
{
class Request
{
private:
	//Request variables
	bool						_is_autoindex;
	bool						_www_auth_required;
	bool						_isCGI;
	int							_type;
	std::string					_language_setted;
	std::string					_file_req;
	std::string					_file_bef_req;
	std::string					_file_type;
	std::string					_request;
	std::string					_http_version;
	std::string					_auth;
	std::string					_realm;
	std::string					_auth_file_path;
	std::string					_client_info;
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
	http::Routes				_location;
	http::ServerConf			_server;
	std::map<int, std::string>	_error_mgs;
	std::vector<std::string>	_allow;
	std::vector<std::string>	_env;
	std::vector<std::string>	_CGI_headers;
	std::vector<std::string>	_custom_headers;
	std::vector<std::string>	_languages_accepted;

	//Response variables
	int			_status;
	std::string _message_status;
	std::string _resp_body;

	// HTTP Methods
	void		build_get(void);
	void		build_put(void);
	void		build_post(void);
	void		build_delete(void);
	void		build_options(void);

	void		startCGI(void);
	void		parent_process(int &pipes_out, int &pipesin_in);
	void		set_status(void);
	void		decode_chunked(void);
	void		save_request(void);
	void		save_header(std::string header);
	void		save_request_body(void);
	void		add_basic_env_vars(void);
	void		get_allowed_methods(void);
	int			decode_CGI_response(void);
	void		get_languages_vector(void);
	void		prepare_status(void);
	bool		needs_auth(http::Routes routes);
	bool		validate_password(std::string auth);
	char		*getResponse(int *size, std::map<std::string, std::string> mime_types);

	std::string custom_header_to_env(std::string custom_header);
	std::string	build_autoindex(void);
	std::string	get_content_type(std::string file_type, std::map<std::string, std::string> mime_types);
	std::map<int, std::string> create_map()
	{
		std::map<int, std::string> m;

		m[200] = "OK";
		m[201] = "Created";
		m[202] = "Accepted";
		m[204] = "No Content";
		m[301] = "Moved Permanently";
		m[302] = "Moved Temporarily";
		m[400] = "Bad Request";
		m[401] = "Unauthorized";
		m[403] = "Forbidden";
		m[404] = "Not Found";
		m[405] = "Not Allowed";
		m[413] = "Payload Too Large";
		m[500] = "Internal Server Error";
		m[505] = "HTTP Version Not Supported";
		return (m);
	};

public:

	Request(std::string req, http::ServerConf server, bool bad_request, std::vector<std::string> env);

	char		*build_response(int *size, std::map<std::string, std::string> mime_types);
};
} // namespace http

#endif