#include "wsv_cgi.h"
#include "wsv_client.h"
#include "wsv_http_special_characters.h"
#include "wsv_tcp.h"
#include "wsv_configuration.h"

#include <string>
#include <iostream>

void
ft_add_header_reponse_(std::string &cgi_string, wsv_http_message_s* m)
{
	if (m->cgi_size >= 0)
	{
		size_t pos = cgi_string.find("\r\n\r\n") + 4;
		cgi_string.replace(pos,
						   cgi_string.size() - pos,
						   cgi_string.size() - pos, '1');


		size_t size = cgi_string.size() - cgi_string.find("\r\n\r\n") - 4;
		//std::cout << size << std::endl;
		cgi_string.insert(0, "HTTP/1.1 200 OK" CRLF
						"Content-Length: " + std::to_string(size) + CRLF);
	}
	else
	{
		size_t size = cgi_string.size() - cgi_string.find("\r\n\r\n") - 4;
		//std::cout << size << std::endl;
		cgi_string.insert(0, "HTTP/1.1 200 OK" CRLF
						"Content-Length: " + std::to_string(size) + CRLF);
	}
}

int
wsv_http_cgi_read(int fd, struct wsv_http_message_s* m)
{
	ssize_t			bytes_read;

	bytes_read = wsv_read(fd, (void*)m->buf_begin, (size_t)m->buf_size);

	if (bytes_read < 0)
	{
		wsv_close(fd);
		return (WSV_ERROR_500);
	}

	if (bytes_read == 0)
	{
		wsv_close(fd);

		if (m->cgi_string.empty())
			return (WSV_ERROR_500);
		else
			ft_add_header_reponse_(m->cgi_string, m);

		m->cgi_begin = m->cgi_string.c_str();
		m->cgi_end = m->cgi_begin + m->cgi_string.size();

		return (WSV_CGI);
	}
	else
	{
		m->cgi_string.append(m->buf_begin, bytes_read);
		return (WSV_RETRY);
	}
}

int
wsv_cgi(char headers[HEADERS_MAX][HEADER_VALUE_SIZE],
		char *clientIp,
		char *pathToCgiExecutable,
		char *requestMethod,
		std::string &requestUri,
		char *scriptname,
		char *queryString,
		int cgi_input,
		void* m)
{
	std::list<std::string>	env;
	char					**env2;

	int						i, j;
	char					*tmp;
	std::string				tmp2;
	int						pid;
	char					*cmd[3];
	std::string				scriptName;
	std::string requestUri_std = requestUri;

	if (scriptname[0] == '.')
	{
		char* buf = getcwd(0, 0);
		if (buf == 0)
			return (WSV_ERROR_500);
		scriptName = buf;
		if (*(scriptName.rbegin()) != '/')
			scriptName.append(scriptname + 1);
		else
			scriptName.append(scriptname + 2);
	}
	else if (scriptname[0] != '/')
	{
		char* buf = getcwd(0, 0);
		if (buf == 0)
			return (WSV_ERROR_500);
		scriptName = buf;
		if (*(scriptName.rbegin()) != '/')
			scriptName.append("/");
		scriptName.append(scriptname);
	}
	else
		scriptName = scriptname;

	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=Webserv");
	env.push_back("REDIRECT_STATUS=200");
	env.push_back(std::string("REMOTE_ADDR=") + clientIp);
	env.push_back(std::string("REQUEST_METHOD=") + requestMethod);

	env.push_back(std::string("CONTENT_LENGTH=") +
				  std::to_string(((wsv_http_message_s*)m)->content_length));

	if (headers[CONTENT_TYPE][0] == ' ')
		env.push_back(std::string("CONTENT_TYPE=") + (&headers[CONTENT_TYPE][1]));
	else
		env.push_back(std::string("CONTENT_TYPE=") + headers[CONTENT_TYPE]);

	tmp2 = std::string("SERVER_NAME=");
	wsv_server_s* srv = (wsv_server_s*)((wsv_http_message_s*)m)->server;
	tmp2.append(srv->ipv6_addr_str);
	env.push_back(tmp2);
	tmp2 = std::string("SERVER_PORT=");
	tmp2.append(srv->port_str);
	env.push_back(tmp2);

	env.push_back(std::string("REQUEST_URI=") + requestUri);
	env.push_back(std::string("SCRIPT_NAME=") + scriptName);

	tmp2 = std::string("PATH_INFO=") + requestUri;
	env.push_back(tmp2);

	tmp2 = std::string("PATH_TRANSLATED=") + scriptName;
	env.push_back(tmp2);

	if(strcmp(headers[10], "") != 0){
		tmp2 = "AUTH_TYPE=";
		for(j=0; headers[AUTHORIZATION][j] != ' '
			&& headers[AUTHORIZATION][j] != '\0';j++);
		tmp2.append(&headers[AUTHORIZATION][0], j);
		env.push_back(tmp2);

		tmp2 = "REMOTE_USER=";
		tmp = strchr(&headers[AUTHORIZATION][0], ' ');
		if(tmp != NULL)
			tmp2.append(tmp + 1);
		env.push_back(tmp2);
	}

	tmp2 = std::string("QUERY_STRING=") + queryString;
	env.push_back(tmp2);

	/* =================================================================== */
	env.splice(env.end(), ((wsv_http_message_s*)m)->cgi_headers);

	size_t bytes = (env.size() + 1) * sizeof(char*);
	env2 = (char**)wsv_malloc(bytes);
	if (env2 == 0)
		return (WSV_ERROR_500);

	//std::cout << "----------------" << std::endl;

	std::list<std::string>::iterator it = env.begin();
	i = 0;
	while(it != env.end())
	{
		env2[i] = (char*)(it->c_str());
		//std::cout << env2[i] << std::endl;
		++it;
		++i;
	}
	env2[i] = NULL;

	cmd[0] = (char*)pathToCgiExecutable;
	cmd[1] = (char*)scriptName.c_str();
	cmd[2] = NULL;

	/* =================================================================== */

	int fdout[2];
	if (wsv_pipe(fdout) < 0)
	{
		wsv_close(cgi_input);
		return (WSV_ERROR_500);
	}

	pid = fork();
	if (pid < 0)
	{
		wsv_close(cgi_input);
		wsv_close(fdout[0]);
		wsv_close(fdout[1]);
		return (WSV_ERROR_500);
	}
	else if (pid == 0)
	{
		wsv_close(fdout[0]);

		if (wsv_dup2(cgi_input, 0) < 0)
		{
			wsv_close(cgi_input);
			wsv_close(fdout[1]);
			exit(0);
		}

		if (wsv_dup2(fdout[1], 1) < 0)
		{
			wsv_close(cgi_input);
			wsv_close(fdout[1]);
			exit(0);
		}

		if (wsv_execve(cmd[0], cmd, env2) == -1)
		{
			wsv_close(cgi_input);
			wsv_close(fdout[1]);
			exit(0);
		}
	}
	else
	{
		wsv_close(cgi_input);
		wsv_close(fdout[1]);
	}

	free(env2);

	wsv_tcp_multiplexing_clear(((wsv_http_message_s*)m)->cgi_input,
							   ((wsv_http_message_s*)m)->fd_sets);
	wsv_client_static_content_read_intialize(fdout[0], (void*)m);
	((wsv_http_message_s*)m)->read_static_content_handler = &wsv_http_cgi_read;
	return (WSV_RETRY);
}
