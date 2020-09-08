#include "Request.hpp"

void http::Request::parent_process(int &pipes_out)
{
	int				valread, max_sd;
	fd_set			readfd;
	char			buffer[BUFFER_SIZE + 1] = {0};
	struct timeval	timeout;

	max_sd = pipes_out;
	FD_ZERO(&readfd);
	FD_SET(pipes_out, &readfd);
	this->_CGI_response = "";
	timeout.tv_sec = 0;
	timeout.tv_usec = 400000;
	while (select(max_sd + 1, &readfd, NULL, NULL, &timeout) > 0)
	{
		if ((valread = read(pipes_out, buffer, BUFFER_SIZE)) < 0)
			continue;
		this->_CGI_response += std::string(buffer, valread);
#ifdef DEBUG_MODE
		std::cout << "CGI_LENGTH->" << this->_CGI_response.length() << std::endl;
#endif
		timeout.tv_sec = 0;
		timeout.tv_usec = 400000;
		FD_ZERO(&readfd);
		FD_SET(pipes_out, &readfd);
	}
}

void http::Request::child_process(char **args, int &pipes_in)
{
	char **env = http::vecToCharptrptr(this->_env);
	int ret = EXIT_SUCCESS;
	int	i = 0;

	if (dup2(pipes_in, STDOUT) < 0)
		throw 500;
	if (this->_request_body.length() > 0)
	{
		this->_CGI_fd = open(FILE_CGI, O_RDONLY, 0);
		if (dup2(this->_CGI_fd, STDIN))
			throw 500;
	}
	else
		close(STDIN);
	if (this->_request_body.length() > 0)
		close(this->_CGI_fd);
	if ((ret = execve(args[0], args, env)) < 0)
		throw 500;
	while (env[i])
		free(env[i++]);
	free(env);
	exit(ret);
}

void http::Request::startCGI(void)
{
	int		ret, status, valwrite;
	int		pipes[2];
	pid_t	pid;
	char	**args;

	if ((this->_CGI_fd = open(FILE_CGI, O_RDWR | O_CREAT | O_TRUNC | O_NOFOLLOW | O_NONBLOCK, 0666)) < 0)
		throw 500;
	valwrite = write(this->_CGI_fd, this->_request_body.c_str(), this->_request_body.length());
	close(this->_CGI_fd);
	if (pipe(pipes))
		throw 500;
	if (!(args = (char **)malloc(sizeof(char *) * 3)))
		throw 500;
	args[0] = strdup(this->_location.getCgiExec().c_str());
	args[1] = strdup(this->_script_name.c_str());
	args[2] = NULL;
	if ((pid = fork()) < 0)
		throw 500;
	else if (pid == 0)
		this->child_process(args, pipes[SIDE_IN]);
	else
	{
		this->parent_process(pipes[SIDE_OUT]);

#ifdef DEBUG_MODE

		std::cerr << "waitpid" << std::endl;

#endif

		waitpid(pid, &status, 0);
	}
	free(args[0]);
	free(args[1]);
	free(args);
	unlink(FILE_CGI);
	close(pipes[SIDE_IN]);
	close(pipes[SIDE_OUT]);
	this->_status = decode_CGI_response();
	if (WIFEXITED(status))
		ret = WEXITSTATUS(status);
}

int http::Request::decode_CGI_response(void)
{
	std::string tmp;
	size_t pos;
	std::stringstream buf;
	int tmp_code;

	if (this->_custom_headers.size() > 0)
	{
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

void http::Request::add_basic_env_vars(void)
{
#ifdef DEBUG_MODE
	int start = (int)this->_env.size();
#endif

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

	for (int i = 0; i < (int)_custom_headers.size(); i++)
		this->_env.push_back(custom_header_to_env(this->_custom_headers[i]));

#ifdef DEBUG_MODE

	std::cout << "Added this env vars:" << std::endl;
	for (int i = start; i < (int)this->_env.size(); i++)
		std::cout << this->_env[i] << std::endl;

#endif
}
