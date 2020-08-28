#include "Request.hpp"
#include "base64.hpp"

void http::Request::save_header(std::string header)
{
	std::vector<std::string> words;
	words = http::split(header, ' ');

	if (words[0] == "Accept-Charsets:")
	{
	}
	else if (words[0] == "Accept-Language:")
	{
		this->_language_header = header.substr(16);
	}
	else if (words[0] == "Authorization:")
	{
		this->_auth = words[2];
	}
	else if (words[0] == "Content-Location:")
	{
	}
	else if (words[0] == "Content-Type:")
	{
		this->_req_content_type = words[1];
	}
	else if (words[0] == "Content-Length:")
	{
		this->_req_content_length = words[1];
	}
	else if (words[0] == "Location:")
	{
	}
	else if (words[0] == "Referer:")
	{
		for (int i = 0; i < (int)words.size(); i++)
			this->_client_info += words[i];
		this->_client_info += "\n";
	}
	else if (words[0] == "Retry-After:")
	{
	}
	else if (words[0] == "Transfer-Encoding:")
	{
		this->_transf_encoding = words[1];
	}
	else if (words[0] == "User-Agent:")
	{
		for (int i = 0; i < (int)words.size(); i++)
			this->_client_info += words[i];
		this->_client_info += "\n";
	}
}

http::Request::Request(std::string req, http::ServerConf server, bool bad_request, std::vector<std::string> env) : _error_mgs(create_map())
{
	std::cout << "************ REQUEST ************" << std::endl;
	std::cout << req << std::endl;
	std::cout << "*********************************" << std::endl;

	this->request = req;
	this->_server = server;
	this->_env = env;
	this->is_autoindex = false;
	this->_isCGI = false;
	this->status = 0;
	this->_auth = "NULL";
	this->_allow.clear();
	this->_www_auth_required = false;
	if (bad_request == true)
		this->status = 400;

	std::vector<std::string> sheader;
	std::vector<std::string> srequest;

	sheader = http::split(req, '\n');

	srequest = http::split(sheader[0], ' ');
	if (srequest[0] == "GET")
		this->type = GET;
	else if (srequest[0] == "HEAD")
		this->type = HEAD;
	else if (srequest[0] == "OPTIONS")
		this->type = OPTIONS;
	else if (srequest[0] == "POST")
		this->type = POST;
	else if (srequest[0] == "PUT")
		this->type = PUT;
	else if (srequest[0] == "DELETE")
		this->type = DELETE;

	this->_req_URI = srequest[1];
	this->http_version = srequest[2];

	for (int i = 1; i < (int)sheader.size(); i++)
	{
		this->save_header(sheader[i]);
	}
	get_languages_vector();

	if (this->_req_URI.find('?') != std::string::npos){
		this->_query_string = this->_req_URI.substr(this->_req_URI.find("?") + 1);
		this->file_bef_req = this->_req_URI.substr(0, this->_req_URI.find("?"));
	}
	else
		this->file_bef_req = this->_req_URI;
	if (this->file_bef_req.find(".") == std::string::npos && this->file_bef_req.back() != '/')
		this->file_bef_req += '/';
	this->location = this->_server.getRoutebyPath(this->file_bef_req); //si file_req = * me pasas la location del server entero
	std::cout << this->location << std::endl;
	std::cout << "File before getFileTransformed: " << this->file_bef_req << std::endl;
	this->file_req = this->location.getFileTransformed(this->file_bef_req, this->_languages_accepted);
	std::cout << "File after getFileTransformed: " << this->file_req << std::endl;
	if (this->file_req.find(".") != std::string::npos)
		this->file_type = this->file_req.substr(file_req.find(".") + 1, file_req.size());
	else if (this->location.getAutoIndex())
		this->is_autoindex = true;
	else
		this->status = 404;
	for (int i = 1; i < (int)sheader.size(); i++)
	{
		this->save_header(sheader[i]);
	}
	if (atoi(this->_req_content_length.c_str()) > 0){
		save_request_body();
	}
	if(this->_transf_encoding.find("chunked") != std::string::npos){
		decode_chunked();
	}
	if (!this->is_autoindex && this->location.isCgi() && this->type != OPTIONS)
	{
		this->_isCGI = true;
		this->_path_info = this->file_req.substr(this->file_req.find(this->location.getExtension()) + 3);
		this->_script_name = this->file_req.substr(0, this->file_req.find(this->location.getExtension()) + 3);
		this->file_req = this->_script_name;
		add_basic_env_vars();
	}
	set_status();
}

void http::Request::add_basic_env_vars(void)
{
	int start = (int)this->_env.size();
	this->_env.push_back("SERVER_NAME=" + this->_server.getServerName());
	this->_env.push_back("SERVER_PORT=" + std::to_string(this->_server.getPort()));
	this->_env.push_back("SERVER_SOFTWARE=webserv/1.0");
	this->_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	this->_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	this->_env.push_back("REDIRECT_STATUS=1");
	this->_env.push_back("REQUEST_URI=" + this->_req_URI);
	if (this->_req_content_type.size() > 0)
		this->_env.push_back("CONTENT_TYPE=" + this->_req_content_type);
	if (this->_req_content_length.size() > 0)
		this->_env.push_back("CONTENT_LENGTH=" + this->_req_content_length);
	else
		this->_env.push_back("CONTENT_TYPE=NULL");
	if (this->_auth != "NULL"){
		this->_env.push_back("AUTH_TYPE=BASIC");
		this->_env.push_back("REMOTE_USER=" + this->_auth);
	}
	if (this->_req_URI.find('?') != std::string::npos){
		this->_env.push_back("QUERY_STRING=" + this->_query_string);
	}
	else{
		this->_env.push_back("QUERY_STRING=");
	}
	if (this->type == GET){
		this->_env.push_back("REQUEST_METHOD=GET");
	}
	else if (this->type == HEAD){
		this->_env.push_back("REQUEST_METHOD=HEAD");
	}
	else if (this->type == POST){
		this->_env.push_back("REQUEST_METHOD=POST");
	}
	else if (this->type == PUT){
		this->_env.push_back("REQUEST_METHOD=PUT");
	}
	else if (this->type == DELETE){
		this->_env.push_back("REQUEST_METHOD=DELETE");
	}
	else if (this->type == OPTIONS){
		this->_env.push_back("REQUEST_METHOD=OPTIONS");
	}
	if (this->_path_info.size() > 0){
		this->_env.push_back("PATH_INFO=" + this->_path_info);
		this->_env.push_back("PATH_TRANSLATED=http://" + this->_server.getServerName() + ":" + std::to_string(this->_server.getPort()) + this->_path_info);
	}
	this->_env.push_back("SCRIPT_NAME=" + this->_script_name);
	this->_env.push_back("SCRIPT_FILENAME=" + this->_script_name);

	std::cout << "Added this env vars:" << std::endl;
	for (int i = start; i < (int)this->_env.size(); i++)
		std::cout << this->_env[i] << std::endl;
	
}

std::string http::Request::get_content_type(std::string type, std::map<std::string, std::string> mime_types)
{
	std::map<std::string, std::string>::iterator iter = mime_types.find(type);
	if (iter == mime_types.end())
	{
		return ("application/octet-stream");
	}
	else
	{
		return (iter->second);
	}
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
	if (this->location.MethodAllow(GET))
		this->_allow.push_back("GET");
	if (this->location.MethodAllow(HEAD))
		this->_allow.push_back("HEAD");
	if (this->location.MethodAllow(POST))
		this->_allow.push_back("POST");
	if (this->location.MethodAllow(PUT))
		this->_allow.push_back("PUT");
	if (this->location.MethodAllow(OPTIONS))
		this->_allow.push_back("OPTIONS");
	if (this->location.MethodAllow(DELETE))
		this->_allow.push_back("DELETE");
	if (this->location.MethodAllow(CONNECT))
		this->_allow.push_back("CONNECT");
	if (this->location.MethodAllow(TRACE))
		this->_allow.push_back("TRACE");
}

void http::Request::set_status(void)
{
	int	code = 0;

	if (this->location.MethodAllow(this->type))
	{
		if (needs_auth(this->location))
		{
			if (this->_auth == "NULL")
			{
				code = 401;
				this->_www_auth_required = true;
			}
			else if (!validate_password(this->_auth))
				code = 403;
		}
		if (!http::file_exists(this->file_req))
		{
			if (code == 0)
			{
				if (this->type == OPTIONS)
					code = 204;
				else
					code = 200;
			}
		}
		else if (http::file_exists(this->file_req) && this->type == PUT)
		{
			if (code == 0)
				code = 201;
		}
		else
		{
			code = 404;
			this->is_autoindex = false;
		}
	}
	else
	{
		code = 405;
		get_allowed_methods();
	}
	if (this->status >= 400)
		code = this->status;
	this->status = code;
	this->message_status = this->_error_mgs[code];
	if (this->status >= 400)
	{
		this->_isCGI = false;
		this->file_req = this->_server.getErrorPage(code);
		this->file_type = this->file_req.substr(file_req.find(".") + 1);
	}
}

char *http::Request::getResponse(int *size, std::map<std::string, std::string> mime_types)
{
	char				*res;
	std::stringstream	stream;

	stream << "HTTP/1.1 " << this->status << " " << this->message_status;
	if (!this->_isCGI && this->status != 204 && this->type != PUT && !this->is_autoindex)
		stream	<< "\nContent-Type: " << get_content_type(this->file_type, mime_types);
	else if (this->is_autoindex)
		stream	<< "\nContent-Type: text/html";
	if (this->status == 405 || (this->status == 204 && this->type == OPTIONS))
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
	if (this->status != 204 && this->type != PUT)
		stream << "\nContent-Length: " << this->resp_body.length();
	stream << "\nServer: Webserv/1.0\n\n";
	if ((this->type != HEAD || this->status != 200) && this->status != 204 && this->type != PUT)
		stream << this->resp_body;
	this->resp_body = stream.str();
	if (this->resp_body.length() < 1000)
	{
		std::cout << "************ RESPONSE ***********" << std::endl;
		std::cout << this->resp_body << std::endl;
		std::cout << "*********************************" << std::endl;
	}
	if (!(res = (char *)malloc(sizeof(char) * (this->resp_body.size() + 1))))
		return (NULL);
	std::copy(this->resp_body.begin(), this->resp_body.end(), res);
	res[this->resp_body.size()] = '\0';
	*size = this->resp_body.size();
	std::cout << "Sending " << this->file_req << std::endl;
	return (res);

}

void	http::Request::build_get(void)
{
	std::ifstream		file;
	std::stringstream	stream;
	std::string			buf;

	if (this->status != 204 && this->status != 201 && !this->_isCGI)
	{
		if (this->is_autoindex)
		{
			DIR				*dirp;
 			struct dirent	*direntp;
			struct stat 	buff;
			std::string		tmp;
			std::string		tmp_p;
			long int		size;
			char			buftime[30];

			stream << "<html>\n<head><title>Index of " << this->file_bef_req <<"</title></head><body>\n";
			stream << "<h1>Index of " << this->file_bef_req << "</h1>" << std::endl;
			stream << "<hr><pre>" << std::endl;
			stream << "<br>" << std::endl;
			dirp = opendir(this->file_req.c_str());
			if (dirp == NULL)
			{
 				std::cerr << "Error opening directory" << this->file_req << std::endl;
 				exit(2);
			}
			while ((direntp = readdir(dirp)) != NULL)
			{
				if (!strncmp(direntp->d_name, ".", strlen(direntp->d_name)))
					continue ;
				tmp = this->file_bef_req + direntp->d_name;
				tmp_p = this->file_req + direntp->d_name;
				if (!strncmp(direntp->d_name, "..", strlen(direntp->d_name)))
					stream << "<a href=\"" << tmp << "\">"<< direntp->d_name << "/</a>";
				else
				{
					stream << "<a href=\"" << tmp << "\">"<< direntp->d_name << "</a>";
					stat(tmp_p.c_str(), &buff);
					ssize_t written = -1;
					struct tm *gm = gmtime(&buff.st_mtimespec.tv_sec);
					if (gm)
					{
						written = (ssize_t)strftime(buftime, sizeof(buftime), "%d-%b-%Y %H:%M", gm);
						if (!(written > 0))
							perror("strftime");
					}
					for (size_t i = 0; i < 20; i++)
						stream << "&nbsp;";
					stream << buftime;
					for (size_t i = 0; i < 8; i++)
						stream << "&nbsp;";
					if (S_ISDIR(buff.st_mode))
						stream << "-";
					else
					{
						size = http::file_size(tmp_p);
						stream << size;
					}
				}
				stream << "<br>" << std::endl;
			}
			stream << "</pre><hr>" << std::endl;
			stream << "</body>\n</html>\n";
			closedir(dirp);
			this->resp_body = stream.str();
		}
		else
		{
			file.open(this->file_req);
			if (file.is_open())
			{
				while (std::getline(file, buf))
					stream << buf << "\n";
				this->resp_body = stream.str();
			}
			file.close();
		}
	}
	if (!this->is_autoindex && this->_isCGI)
		startCGI();
}

void	http::Request::build_post(void)
{
	if (this->_isCGI)
		startCGI();
}

void	http::Request::build_put(void)
{
	std::ofstream	f(this->file_req);

	if (f.good())
		f << this->_request_body << std::endl;
	else
		f << "error" << std::endl;
	f.close();
}

void	http::Request::build_delete(void)
{
	std::stringstream	stream;

	unlink(this->file_req.c_str());
	stream << "<html>\n<body>\n<h1>File deleted.</h1>\n</body>\n</html>" << std::endl;
	this->resp_body = stream.str();
}

void	http::Request::build_options(void)
{
	get_allowed_methods();
}

char *http::Request::build_response(int *size, std::map<std::string, std::string> mime_types)
{
	if (this->type == GET || this->status >= 400)
		this->build_get();
	else if (this->type == POST)
		this->build_post();
	else if (this->type == PUT && !this->_isCGI)
		this->build_put();
	else if (this->type == DELETE)
		this->build_delete();
	else if (this->type == OPTIONS)
		this->build_options();
	return (getResponse(size, mime_types));
}

void http::Request::startCGI(void){
	char **env = http::vecToCharptrptr(this->_env);
	char **args;
	int ret = EXIT_SUCCESS;
	int status;
	int pipes[2];
	int pipes_in[2];
	if (pipe(pipes)){
		perror("pipe");
	}
	if (this->_req_content_length.size() > 0){
		if (pipe(pipes_in)){
			perror("pipe");
		}
	}
	if (!(args = (char **)malloc(sizeof(char *) * 2))){
			perror("malloc");
	}
	args[0] = strdup(this->location.getCgiExec().c_str());
	args[1] = NULL;

	pid_t pid = fork();
	if (pid < 0){
		perror("fork");
	}
	else if (pid == 0){
		if (dup2(pipes[SIDE_IN], STDOUT) < 0){
			perror("dup2");
		}
		if (this->_req_content_length.size() > 0){
			if (dup2(pipes_in[SIDE_OUT], STDIN) < 0){
				perror("dup2");
			}
			write(pipes_in[SIDE_IN], this->_request_body.c_str(), this->_request_body.size());
			close(pipes_in[SIDE_IN]);
		}
		if ((ret = execve(args[0], args, env)) < 0){
			perror("execve");
		}
		exit(ret);
	}
	else {
		waitpid(pid, &status, 0);
		close(pipes[SIDE_IN]);
		if (this->_req_content_length.size() > 0){
			close(pipes_in[SIDE_OUT]);
		}
		char buffer[30000] = {0};
		read(pipes[SIDE_OUT], buffer, 30000);
		this->_CGI_response = buffer;
		decode_CGI_response();
		close(pipes[SIDE_OUT]);
		if(WIFEXITED(status)){
			ret = WEXITSTATUS(status);
		}
	}

}

bool http::Request::validate_password(std::string auth)
{
	std::ifstream htpasswd;
	std::string buf;

	auth = auth.substr(0, auth.size() - 1);
	std::vector<std::uint8_t> decoded = base64::decode(auth);
	std::string decoded_str;
	for (int i = 0; i < (int)decoded.size(); i++)
	{
		decoded_str += (char)decoded[i];
	}
	int pos = decoded_str.find(':');
	std::string user;
	std::string password;
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
				{
					decoded.push_back((unsigned char)password[i]);
				}
				std::string encoded = base64::encode(decoded);
				if (encoded == encrypted_passwd)
				{
					return (true);
				}
			}
		}
	}
	return (false);
}

void http::Request::save_request_body(void){
	for (int i = 0; i < (int)this->request.length(); i++)
	{
		if (this->request[i] == '\n' && this->request[i + 2] == '\n')
		{
			this->_request_body =  this->request.substr(i + 3, atoi(this->_req_content_length.c_str()));
		}
	}
}

void http::Request::decode_chunked(void){
	std::string tmp;
	std::vector<std::string> chunked_vec;
	for (int i = 0; i < (int)this->request.length(); i++){
		if (this->request[i] == '\n' && this->request[i + 2] == '\n')
		{
			chunked_vec = http::split(this->request.substr(i + 3), '\n');
			break;
		}
	}
	int i = 0;
	int ret = 0;
	int body_length = 0;;
	while((ret = stoi(chunked_vec[i], 0, 16)) > 0){
		body_length += ret;
		this->_request_body += chunked_vec[++i].substr(0, ret);
		i++;
	}
	this->_req_content_length = std::to_string(body_length);
}

void http::Request::decode_CGI_response(void){
	std::string tmp;

	while(this->_CGI_response.find('\n') != std::string::npos){
		tmp = this->_CGI_response.substr(0, this->_CGI_response.find('\n'));
		this->_CGI_response = this->_CGI_response.substr(this->_CGI_response.find('\n') + 1);
		if (tmp.length() == 1 && tmp[0] == '\r'){
			this->resp_body = this->_CGI_response;
			break;
		}
		this->_CGI_headers.push_back(tmp);
	}
}

void http::Request::get_languages_vector(void){
	this->_languages_accepted = http::split(this->_language_header, ',');
	for(int i = 0; i < (int)this->_languages_accepted.size(); i++){
		int min = 0;
		if (this->_languages_accepted[i][0] == ' ')
			min = 1;
		if (this->_languages_accepted[i].find(';') != std::string::npos){
			this->_languages_accepted[i] = this->_languages_accepted[i].substr(min, this->_languages_accepted[i].find(';') - 1);
		}
		else
			if (this->_languages_accepted[i][this->_languages_accepted[i].length() - 1] == '\r')
				this->_languages_accepted[i] = this->_languages_accepted[i].substr(min, this->_languages_accepted[i].length() - 1 - min);
			else{
				this->_languages_accepted[i] = this->_languages_accepted[i].substr(min);
			}
		std::cout << "language: " << this->_languages_accepted[i] << std::endl;
	}
}
