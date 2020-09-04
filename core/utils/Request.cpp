#include "Request.hpp"
#include "base64.hpp"

http::Request::Request(std::string req, http::ServerConf server, bool bad_request, std::vector<std::string> env):
						_is_autoindex(false),
						_www_auth_required(false),
						_isCGI(false),
						_request(req),
						_auth("NULL"),
						_server(server),
						_error_mgs(create_map()),
						_env(env),
						_status(0)
{
	std::cout << "************ REQUEST HEADERS ************" << std::endl;
	std::cout << req.substr(0, req.find("\r\n\r\n")) << std::endl;
	std::cout << "*********************************" << std::endl;
	std::cout << "REQUEST LENGTH: " << req.length() << std::endl;
	this->_allow.clear();
	if (bad_request == true)
		this->_status = 400;
	this->save_request();
	if (!this->_is_autoindex && this->_location.isCgi() && this->_type != OPTIONS && !this->_location.isRedirect())
	{
		this->_isCGI = true;
		std::cout << "extension =" << this->_location.getExtension() << std::endl;
		this->_path_info = this->_file_req.substr(this->_file_req.find(this->_location.getExtension()) + 3);
		this->_script_name = this->_file_req.substr(0, this->_file_req.find(this->_location.getExtension()) + 3);
		this->_file_req = this->_script_name;
		add_basic_env_vars();
		std::cout << "File after CGI Transformation: " << this->_file_req << std::endl;
	}
	if (!this->_location.isRedirect())
	{
		if (!http::is_dir(this->_file_req) || this->_type == PUT)
			this->_file_type = this->_file_req.substr(_file_req.find(".") + 1, _file_req.size());
		else if (this->_location.getAutoIndex())
			this->_is_autoindex = true;
	}
	set_status();
	prepare_status();
}

void	http::Request::save_request()
{
	std::vector<std::string> sheader;
	std::vector<std::string> srequest;

	sheader = http::split(this->_request.substr(0, this->_request.find("\r\n\r\n")), '\n');
	srequest = http::split(sheader[0], ' ');
	if (srequest[0] == "GET")
		this->_type = GET;
	else if (srequest[0] == "HEAD")
		this->_type = HEAD;
	else if (srequest[0] == "OPTIONS")
		this->_type = OPTIONS;
	else if (srequest[0] == "POST")
		this->_type = POST;
	else if (srequest[0] == "PUT")
		this->_type = PUT;
	else if (srequest[0] == "DELETE")
		this->_type = DELETE;
	this->_req_URI = srequest[1];
	this->_http_version = srequest[2];
	for (int i = 1; i < (int)sheader.size(); i++)
		this->save_header(sheader[i]);
	if (atoi(this->_req_content_length.c_str()) > 0)
		save_request_body();
	if (this->_transf_encoding.find("chunked") != std::string::npos)
		decode_chunked();
	get_languages_vector();
	if (this->_req_URI.find('?') != std::string::npos)
	{
		this->_query_string = this->_req_URI.substr(this->_req_URI.find("?") + 1);
		this->_file_bef_req = this->_req_URI.substr(0, this->_req_URI.find("?"));
	}
	else
		this->_file_bef_req = this->_req_URI;
	if (this->_file_bef_req.find(".") == std::string::npos && this->_file_bef_req.back() != '/' && this->_type != PUT && this->_type != POST)
		this->_file_bef_req += '/';
	this->_location = this->_server.getRoutebyPath(this->_file_bef_req);
	std::cout << "Location: " << this->_location.getVirtualLocation() << std::endl;
	if (this->_location.isRedirect())
		this->_file_bef_req = this->_location.getPathRedirect();
	else
	{
		std::cout << "File before getFileTransformed: " << this->_file_bef_req << std::endl;
		this->_file_req = this->_location.getFileTransformed(this->_file_bef_req, this->_languages_accepted, this->_type, this->_language_setted);
		std::cout << "File after getFileTransformed: " << this->_file_req << std::endl;
	}
}

void http::Request::save_header(std::string header)
{
	std::vector<std::string> words = http::split(header, ' ');

	if (words[0] == "Accept-Charsets:")
	{
	}
	if (words[0] == "Accept-Encoding:")
	{
	}
	else if (words[0] == "Accept-Language:")
		this->_language_header = header.substr(16, header.length() - 17);
	else if (words[0] == "Authorization:")
		this->_auth = words[2];
	else if (words[0] == "Content-Location:")
	{
	}
	else if (words[0] == "Content-Type:")
		this->_req_content_type = words[1];
	else if (words[0] == "Content-Length:")
		this->_req_content_length = header.substr(16, header.length() - 17);
	else if (words[0] == "Location:")
	{
	}
	else if (words[0] == "Referer:" || words[0] == "User-Agent:")
	{
		for (int i = 0; i < (int)words.size(); i++)
			this->_client_info += words[i];
		this->_client_info += "\n";
	}
	else if (words[0] == "Transfer-Encoding:")
		this->_transf_encoding = words[1];
	else if (words[0] == "Host:")
	{
	}
	else if (words[0][0] == 'X' && words[0][1] == '-'){
		this->_custom_headers.push_back(header);
	}
}

void http::Request::save_request_body(void)
{
	for (int i = 0; i < (int)this->_request.length(); i++){
		if (this->_request[i] == '\n' && this->_request[i + 2] == '\n'){
			this->_request_body = this->_request.substr(i + 3, atoi(this->_req_content_length.c_str()));
			break;
		}
	}
	
}

void http::Request::set_status(void)
{
	int code = 0;

	if (!this->_http_version.compare("HTTP/1.1"))
		this->_status = 505;
	if (!this->_status && this->_location.MethodAllow(this->_type))
	{
		if (needs_auth(this->_location))
		{
			if (this->_auth == "NULL")
			{
				code = 401;
				this->_www_auth_required = true;
			}
			else if (!validate_password(this->_auth))
				code = 403;
		}
		if (this->_location.isRedirect()){
			code = this->_location.getCodeRedirect();
		}
		else if (this->_request_body.length() > this->_location.getBodySize() && !code)
			code = 413;
		else if (!http::file_exists(this->_file_req) && !http::is_dir(this->_file_req))
		{
			if (!code)
			{
				if (this->_type == OPTIONS)
					code = 204;
				else
					code = 200;
			}
		}
		else if (http::file_exists(this->_file_req) && (this->_type == PUT or (this->_type == POST && !this->_isCGI)) && !code){
			code = 201;
		}
		else if (http::file_exists(this->_file_req) && this->_type == POST && this->_isCGI && !code){
			code = 200;
		}
		//else if (http::is_dir(this->_file_req))
		//	code = 403;
		else
		{
			code = 404;
			this->_is_autoindex = false;
		}
	}
	else if (!this->_status)
	{
		code = 405;
		get_allowed_methods();
	}
	if (!this->_status)
		this->_status = code;
}

void		http::Request::prepare_status()
{
	this->_message_status = this->_error_mgs[this->_status];
	if (this->_status >= 400)
	{
		this->_isCGI = false;
		this->_file_req = this->_server.getErrorPage(this->_status);
		this->_file_type = this->_file_req.substr(_file_req.find(".") + 1);
	}
}

void http::Request::add_basic_env_vars(void)
{
	int start = (int)this->_env.size();

	this->_env.push_back("SERVER_NAME=" + this->_server.getServerName());
	this->_env.push_back("SERVER_PORT=" + std::to_string(this->_server.getPort()));
	this->_env.push_back("SERVER_SOFTWARE=webserv/1.0");
	this->_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	this->_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	this->_env.push_back("REDIRECT_STATUS=200");
	this->_env.push_back("REQUEST_URI=" + this->_req_URI);
	if (this->_req_content_type.size() > 0)
		this->_env.push_back("CONTENT_TYPE=" + this->_req_content_type);
	if (this->_req_content_length.size() > 0)
		this->_env.push_back("CONTENT_LENGTH=" + this->_req_content_length);
	else
		this->_env.push_back("CONTENT_LENGTH=0");
	if (this->_auth != "NULL")
	{
		this->_env.push_back("AUTH_TYPE=BASIC");
		this->_env.push_back("REMOTE_USER=" + this->_auth);
	}
	if (this->_req_URI.find('?') != std::string::npos)
		this->_env.push_back("QUERY_STRING=" + this->_query_string);
	else
		this->_env.push_back("QUERY_STRING=");
	if (this->_type == GET)
		this->_env.push_back("REQUEST_METHOD=GET");
	else if (this->_type == HEAD)
		this->_env.push_back("REQUEST_METHOD=HEAD");
	else if (this->_type == POST)
		this->_env.push_back("REQUEST_METHOD=POST");
	else if (this->_type == PUT)
		this->_env.push_back("REQUEST_METHOD=PUT");
	else if (this->_type == DELETE)
		this->_env.push_back("REQUEST_METHOD=DELETE");
	else if (this->_type == OPTIONS)
		this->_env.push_back("REQUEST_METHOD=OPTIONS");

	this->_env.push_back("PATH_INFO=" + this->_req_URI);
	this->_env.push_back("PATH_TRANSLATED=" + this->_script_name);
	this->_env.push_back("SCRIPT_NAME=" + this->_script_name);
	this->_env.push_back("SCRIPT_FILENAME=" + this->_script_name);
	this->_env.push_back("REMOTE_ADDR=127.0.0.1");
	this->_env.push_back("REMOTE_IDENT=local");

	for(int i = 0; i < (int)_custom_headers.size(); i++){
		this->_env.push_back(custom_header_to_env(this->_custom_headers[i]));
	}

	std::cout << "Added this env vars:" << std::endl;
	for (int i = start; i < (int)this->_env.size(); i++)
		std::cout << this->_env[i] << std::endl;
}

std::string http::Request::get_content_type(std::string type, std::map<std::string, std::string> mime_types)
{
	std::map<std::string, std::string>::iterator iter = mime_types.find(type);

	if (iter == mime_types.end())
		return ("application/octet-stream");
	else
		return (iter->second);
}

bool http::Request::needs_auth(http::Routes route)
{
	if (route.needAuth())
	{
		this->_realm = route.getAuthMessage();
		this->_auth_file_path = route.getPassAuthFile();
		return (true);
	}
	else if (!route.needExplicitAuth() && this->_server.needAuth())
	{
		this->_realm = this->_server.getAuthMessage();
		this->_auth_file_path = this->_server.getPassAuthFile();
		return (true);
	}
	return (false);
}

void http::Request::get_allowed_methods(void)
{
	if (this->_location.MethodAllow(GET))
		this->_allow.push_back("GET");
	if (this->_location.MethodAllow(HEAD))
		this->_allow.push_back("HEAD");
	if (this->_location.MethodAllow(POST))
		this->_allow.push_back("POST");
	if (this->_location.MethodAllow(PUT))
		this->_allow.push_back("PUT");
	if (this->_location.MethodAllow(OPTIONS))
		this->_allow.push_back("OPTIONS");
	if (this->_location.MethodAllow(DELETE))
		this->_allow.push_back("DELETE");
	if (this->_location.MethodAllow(CONNECT))
		this->_allow.push_back("CONNECT");
	if (this->_location.MethodAllow(TRACE))
		this->_allow.push_back("TRACE");
}

char *http::Request::getResponse(int *size, std::map<std::string, std::string> mime_types)
{
	char *res;
	std::stringstream stream;

	stream << "HTTP/1.1 " << this->_status << " " << this->_message_status;
	if (!this->_isCGI && this->_status != 204 && this->_type != PUT && !this->_is_autoindex)
		stream << "\nContent-Type: " << get_content_type(this->_file_type, mime_types);
	else if (this->_is_autoindex)
		stream << "\nContent-Type: text/html";
	if (this->_status == 405 || (this->_status == 204 && this->_type == OPTIONS))
	{
		stream << "\nAllow:";
		for (int i = 0; i < (int)this->_allow.size(); i++)
		{
			if (i < (int)this->_allow.size() - 1)
				stream << " " << this->_allow[i] << ",";
			else
				stream << " " << this->_allow[i];
		}
	}
	if (this->_www_auth_required == true)
		stream << "\nWWW-Authenticate: Basic realm=\"" << this->_realm << "\"";
	if (this->_isCGI)
		for (int i = 0; i < (int)this->_CGI_headers.size(); i++)
			stream << "\n" << this->_CGI_headers[i];
	stream << "\nDate: " << http::get_actual_date();
	if (this->_status != 204 && this->_type != PUT)
		stream << "\nContent-Length: " << this->_resp_body.length();
	else
		stream << "\nContent-Length: 0";
	if (this->_language_setted.length() > 0)
		stream << "\nContent-Language: " << this->_language_setted;
	if (this->_status == 201 or this->_status == 301 or this->_status == 302)
		stream << "\nLocation: " << this->_file_bef_req;
	stream << "\nServer: Webserv/1.0\r\n\r\n";
	if (this->_type != HEAD && this->_status != 204 && this->_type != PUT)
		stream << this->_resp_body;
	this->_resp_body = stream.str();
	if (this->_resp_body.length() < 1000)
	{
		std::cout << "************ RESPONSE ***********" << std::endl;
		std::cout << this->_resp_body;
		std::cout << "*********************************" << std::endl;
	}
	if (!(res = (char *)malloc(sizeof(char) * (this->_resp_body.size() + 1))))
		return (NULL);
	std::copy(this->_resp_body.begin(), this->_resp_body.end(), res);
	res[this->_resp_body.size()] = '\0';
	*size = this->_resp_body.size();
	std::cout << "Sending " << this->_file_req << std::endl;
	return (res);
}

std::string	http::Request::build_autoindex(void)
{
	std::string			tmp;
	std::string			tmp_p;
	std::stringstream	stream;
	char				buftime[30];
	DIR					*dirp;
	struct dirent		*direntp;
	struct tm			*gm;
	struct stat			buff;

	stream << "<html>\n<head><title>Index of " << this->_file_bef_req << "</title></head><body>\n";
	stream << "<h1>Index of " << this->_file_bef_req << "</h1><hr><pre><br>" << std::endl;
	dirp = opendir(this->_file_req.c_str());
	if (dirp == NULL)
		perror("opendir");
	while ((direntp = readdir(dirp)) != NULL)
	{
		if (!strncmp(direntp->d_name, ".", strlen(direntp->d_name)))
			continue;
		tmp = this->_file_bef_req + direntp->d_name;
		tmp_p = this->_file_req + direntp->d_name;
		if (!strncmp(direntp->d_name, "..", strlen(direntp->d_name)))
			stream << "<a href=\"" << tmp << "\">" << direntp->d_name << "/</a>";
		else
		{
			stream << "<a href=\"" << tmp << "\">" << direntp->d_name << "</a>";
			stat(tmp_p.c_str(), &buff);
			ssize_t written = -1;
			gm = gmtime(&buff.st_mtimespec.tv_sec);
			if (gm)
				if (!((written = (ssize_t)strftime(buftime, sizeof(buftime), "%d-%b-%Y %H:%M", gm)) > 0))
					perror("strftime");
			for (size_t i = 0; i < 20; i++)
				stream << "&nbsp;";
			stream << buftime;
			for (size_t i = 0; i < 8; i++)
				stream << "&nbsp;";
			if (S_ISDIR(buff.st_mode))
				stream << "-";
			else
				stream << http::file_size(tmp_p);
		}
		stream << "<br>" << std::endl;
	}
	closedir(dirp);
	stream << "</pre><hr></body>\n</html>\n" << std::endl;
	return (stream.str());
}

void http::Request::build_get(void)
{
	if (this->_status != 204 && this->_status != 201 && !this->_isCGI)
	{
		if (this->_is_autoindex)	
			this->_resp_body = build_autoindex();
		else
			this->_resp_body = http::file_content(this->_file_req);
	}
	if (!this->_is_autoindex && this->_isCGI)
		startCGI();
}

void http::Request::build_post(void)
{
	if (this->_isCGI)
		startCGI();
	else
	{
		std::ofstream f(this->_file_req, std::ios::app);

		if (f.good())
			f << this->_request_body << std::endl;
		else
			f << "error" << std::endl;
		f.close();
	}
}

void http::Request::build_put(void)
{
	std::ofstream f(this->_file_req);

	if (f.good())
		f << this->_request_body << std::endl;
	else
		f << "error" << std::endl;
	f.close();
}

void http::Request::build_delete(void)
{
	std::stringstream stream;

	unlink(this->_file_req.c_str());
	stream << "<html>\n<body>\n<h1>File deleted.</h1>\n</body>\n</html>" << std::endl;
	this->_resp_body = stream.str();
}

void http::Request::build_options(void)
{
	get_allowed_methods();
}

char *http::Request::build_response(int *size, std::map<std::string, std::string> mime_types)
{
	if (this->_type == GET || this->_status >= 400)
		this->build_get();
	else if (this->_type == POST)
		this->build_post();
	else if (this->_type == PUT && !this->_isCGI)
		this->build_put();
	else if (this->_type == DELETE)
		this->build_delete();
	else if (this->_type == OPTIONS)
		this->build_options();
	if (this->_status >= 500)
		prepare_status();
	return (getResponse(size, mime_types));
}

void	http::Request::parent_process(int &pipes_out, int &pipesin_in)
{
	(void)pipesin_in;
	int		valread, max_sd;
	fd_set	readfd;
	char    buffer[BUFFER_SIZE + 1] = {0};
	struct timeval timeout;

	
	max_sd = pipes_out;
	FD_ZERO(&readfd);
	FD_SET(pipes_out, &readfd);
	this->_CGI_response = "";
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000;
	while (select(max_sd + 1, &readfd, NULL, NULL, &timeout) > 0)
	{
		if ((valread = read(pipes_out, buffer, BUFFER_SIZE)) < 0)
			continue ;
		this->_CGI_response += std::string(buffer, valread);
		timeout.tv_sec = 0;
		timeout.tv_usec = 500000;
		FD_ZERO(&readfd);
		FD_SET(pipes_out, &readfd);
	}
}

void http::Request::startCGI(void)
{
    int     ret = EXIT_SUCCESS;
    int		status;
    int     pipes[2];
    //int     pipes_in[2];
    pid_t   pid;
    char    **env = http::vecToCharptrptr(this->_env);
    char    **args;
	int		valwrite;

	this->_CGI_fd = open("tmp/CGI.tmp", O_RDWR | O_CREAT | O_TRUNC | O_NOFOLLOW | O_NONBLOCK, 0666);
	valwrite = write(this->_CGI_fd, this->_request_body.c_str(), this->_request_body.length());
	close(this->_CGI_fd);
    if (pipe(pipes))
        perror("pipe");
    if (!(args = (char **)malloc(sizeof(char *) * 3)))
        perror("malloc");
    args[0] = strdup(this->_location.getCgiExec().c_str());
    args[1] = strdup(this->_script_name.c_str());
    args[2] = NULL;
    if ((pid = fork()) < 0)
        perror("fork");
    else if (pid == 0)
    {
        if (dup2(pipes[SIDE_IN], STDOUT) < 0)
            perror("dup2");
        if (this->_request_body.length() > 0){
            this->_CGI_fd = open("tmp/CGI.tmp", O_RDONLY, 0);
			if (dup2(this->_CGI_fd, STDIN)){
				perror("dup2");
			}
		}
		else{
			close(STDIN);
		}
        if ((ret = execve(args[0], args, env)) < 0)
            perror("execve");
        free(args);
		free(env);
        exit(ret);
    }
    else
    {
		this->parent_process(pipes[SIDE_OUT], this->_CGI_fd);
		std::cerr << "waitpid" << std::endl;
		waitpid(pid, &status, 0);
    }
	close(pipes[SIDE_IN]);
	close(pipes[SIDE_OUT]);
	this->_status = decode_CGI_response();
	if (WIFEXITED(status))
		ret = WEXITSTATUS(status);
}


bool http::Request::validate_password(std::string auth)
{
	std::ifstream htpasswd;
	std::string buf;
	std::string decoded_str;
	std::string user;
	std::string password;

	auth = auth.substr(0, auth.size() - 1);
	std::vector<std::uint8_t> decoded = base64::decode(auth);
	for (int i = 0; i < (int)decoded.size(); i++)
		decoded_str += (char)decoded[i];
	int pos = decoded_str.find(':');
	user = decoded_str.substr(0, pos);
	password = decoded_str.substr(pos + 1, (decoded_str.size() - pos + 1));
	htpasswd.open(this->_auth_file_path);
	if (htpasswd.is_open())
	{
		while (std::getline(htpasswd, buf))
		{
			if (user == buf.substr(0, buf.find(':')))
			{
				std::string encrypted_passwd = buf.substr(buf.find(':') + 1, buf.size() - buf.find(':') + 1);
				decoded.clear();
				for (int i = 0; i < (int)password.size(); i++)
					decoded.push_back((unsigned char)password[i]);
				std::string encoded = base64::encode(decoded);
				if (encoded == encrypted_passwd)
					return (true);
			}
		}
	}
	return (false);
}

void http::Request::decode_chunked(void)
{
	std::cout << "se mete en decode chunked" << std::endl;
	int ret = 0;
	std::string tmp;

	std::cout << "hace substr" << std::endl;
	tmp = this->_request.substr(this->_request.find("\r\n\r\n") + 4);

	std::stringstream ss(tmp);
	std::string tok;
	std::cout << "bucle" << std::endl;
	while (std::getline(ss, tok, '\n'))
	{
		if ((ret = std::stoi(tok, 0, 16)) == 0){
			break;
		}
		std::getline(ss, tok, '\n');
		tok.erase(tok.end() - 1);
		this->_request_body.append(tok);
	}
	this->_req_content_length = std::to_string(this->_request_body.length());
	std::cout << "sale de decode chunked" << std::endl;
}

int http::Request::decode_CGI_response(void)
{
	std::string			tmp;
	size_t				pos;
	std::stringstream	buf;
	int					tmp_code;

	if (this->_custom_headers.size() > 0){
		size_t pos = this->_CGI_response.find("\r\n\r\n") + 4;
		this->_CGI_response.replace(pos,
							this->_CGI_response.size() - pos,
						   	this->_CGI_response.size() - pos, '1');
	}

	while (this->_CGI_response.find('\n') != std::string::npos)
	{
		tmp = this->_CGI_response.substr(0, this->_CGI_response.find('\n'));
		pos = tmp.find("Status: ");
		if (pos != std::string::npos)
		{
			while (std::isdigit(tmp[pos]))
				buf << tmp[pos++];
		}
		this->_CGI_response = this->_CGI_response.substr(this->_CGI_response.find('\n') + 1);
		if (tmp.length() == 1 && tmp[0] == '\r')
		{
			this->_resp_body = this->_CGI_response;
			break;
		}
		this->_CGI_headers.push_back(tmp);
	}
	tmp_code = atoi(buf.str().c_str());
	if (!tmp_code)
		tmp_code = this->_status;
	return (tmp_code);
}

void http::Request::get_languages_vector(void)
{
	int min;

	this->_languages_accepted = http::split(this->_language_header, ',');
	for (int i = 0; i < (int)this->_languages_accepted.size(); i++)
	{
		min = 0;
		std::cout << "language antes de tratarlo: " << this->_languages_accepted[i] << std::endl;
		if (this->_languages_accepted[i][0] == ' ')
			min = 1;
		if (this->_languages_accepted[i].find(';') != std::string::npos)
			this->_languages_accepted[i] = this->_languages_accepted[i].substr(min, this->_languages_accepted[i].find(';'));
		else
			this->_languages_accepted[i] = this->_languages_accepted[i].substr(min);
		std::cout << "language: " << this->_languages_accepted[i] << std::endl;
	}
}

std::string http::Request::custom_header_to_env(std::string custom_header){
	std::string key = custom_header.substr(0, custom_header.find(':'));
	std::string value = custom_header.substr(custom_header.find(':') + 1);

	for(int i = 0; i < (int)key.length(); i++){
		if (key[i] >= 'a' && key[i] <= 'z'){
			key[i] = std::toupper(key[i]);
		}
		if(key[i] == '-')
			key[i] = '_';
	}
	key += "=";
	for(int i = 0; i < (int)value.length(); i++){
		if (value[i] != ' ' && value[i] != '\r'){
			key += value[i];
		}
	}
	std::cout << "adding env var: " << key << std::endl;
	return (key);
}
