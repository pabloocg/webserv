#include <iostream>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_client.h"
# include "wsv_http_message.h"
#include "wsv_configuration.h"
#include "wsv_http_header.h"
#include "wsv_string.h"
#include "wsv_tcp.h"
#include "wsv_http_special_characters.h"
#include "wsv_error_log.h"

static int
wsv_http_cgi_response(std::string& cgi_string, int cgi_extensions)
{
	size_t			pos;
	size_t			len;
	std::string		status_line("HTTP/1.1 200 OK" CRLF);
	std::string		content_length;

	try {
		if ((pos = cgi_string.find(CRLF_CRLF)) != std::string::npos)
		{
			pos += 4;
			len = cgi_string.size() - pos;
			if (cgi_extensions != 0)
				cgi_string.replace(pos, len, len, '1');
			do
			{
				content_length.insert(0, 1, (char)('0' + (len % 10)));
				len /= 10;
			} while (len != 0);
			content_length.insert(0, "Content-Length: ");
			content_length.append(CRLF);
		}
		else if ((pos = pos = cgi_string.find("\n\n")) != std::string::npos)
		{
			pos += 2;
			len = cgi_string.size() - pos;
			if (cgi_extensions != 0)
				cgi_string.replace(pos, len, len, '1');
			do
			{
				content_length.insert(0, 1, (char)('0' + (len % 10)));
				len /= 10;
			} while (len != 0);
			content_length.insert(0, "Content-Length: ");
			content_length.append(CRLF);
		}
		else
			return (WSV_ERROR);

		pos = cgi_string.find("Status:");
		if (pos != std::string::npos)
		{
			pos += 7;

			len = cgi_string.find(CRLF);
			if (len == std::string::npos)
				len = cgi_string.find("\n");

			status_line = cgi_string.substr(pos, (len - pos));
			status_line.insert(0, "HTTP/1.1 ");
			status_line.append(CRLF);
		}

		cgi_string.insert(0, status_line + content_length);

		return (WSV_OK);
	}
	catch (const std::exception& e) {
		std::cerr << "\033[0;31m"
				  <<"wsv_http_cgi_response(): "
				  << e.what()
				  << "\033[0;0m"
				  << std::endl;
		return (WSV_ERROR);
	}
}

static int
wsv_http_cgi_read(int fd, struct wsv_http_message_s* m)
{
	ssize_t			bytes_read;

	bytes_read = wsv_read(fd, (void*)m->buf_begin, (size_t)m->buf_size);

	if (bytes_read < 0)
	{
		wsv_close(fd); /* insert error check */
		m->cgi_string.clear();
		wsv_tcp_multiplexing_cgi_clear(m->fd_sets);
		return (WSV_ERROR_500);
	}

	try {
		if (bytes_read == 0)
		{
			wsv_close(fd); /* insert error check */
			wsv_waitpid(m->cgi_pid, 0, 0); /* to remove zombie child process */

			if (m->cgi_string.empty())
			{
				wsv_tcp_multiplexing_cgi_clear(m->fd_sets);
				return (WSV_ERROR_500);
			}
			else
			{
				if (wsv_http_cgi_response(m->cgi_string,
										  m->cgi_extensions) == WSV_ERROR)
				{
					m->cgi_string.clear();
					wsv_tcp_multiplexing_cgi_clear(m->fd_sets);
					return (WSV_ERROR_500);
				}
			}

			m->cgi_begin = m->cgi_string.c_str();
			m->cgi_end = m->cgi_begin + m->cgi_string.size();

#ifdef WSV_LOG
			write(STDOUT_FILENO, "\n+++ CGI Response ++++\n", 23);
			if (m->cgi_string.size() <= 1024)
				write(STDOUT_FILENO, m->cgi_begin, m->cgi_string.size());
			else
				write(STDOUT_FILENO, m->cgi_begin, 1024);
			write(STDOUT_FILENO, "\n+++++++++++++++++++++\n", 23);
#endif

			wsv_tcp_multiplexing_cgi_clear(m->fd_sets);
			return (WSV_CGI);
		}
		else
		{
			m->cgi_string.append(m->buf_begin, bytes_read);
			return (WSV_RETRY);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "\033[0;31m"
				  << "wsv_http_cgi_read(): "
				  << e.what()
				  << "\033[0;0m"
				  << std::endl;
		wsv_close(fd); /* insert error check */
		wsv_tcp_multiplexing_cgi_clear(m->fd_sets);
		return (WSV_ERROR_500);
	}
}

static int
wsv_http_cgi_exec(struct wsv_http_message_s* m,
				  std::vector<std::string>& variables)
{
	pid_t						pid;
	char*						argv[3];
	char**						envp;
	size_t						n;
	int							pipefd[2];

	n = variables.size() + 1;
	envp = (char**)wsv_malloc(sizeof(char*) * n);
	if (envp == 0)
	{
		wsv_close(m->cgi_fd); /* insert error check */
		wsv_tcp_multiplexing_cgi_clear(m->fd_sets);
		return (WSV_ERROR_500);
	}

	envp[--n] = 0;
	while (n--)
		envp[n] = (char*)variables[n].c_str();

	argv[0] = (char*)m->cgi_bin;
	argv[1] = (char*)m->cgi_target_ressource;
	argv[2] = 0;

#ifdef WSV_LOG
		std::cout << "\n++++ CGI envp ++++\n";
		for (n = 0; envp[n]; ++n)
			std::cout << envp[n] << '\n';
		std::cout << "\n++ CGI argv ++\n";
		for (n = 0; argv[n]; ++n)
			std::cout << argv[n] << '\n';
		std::cout << "++++++++++++++++++++++++++++" << std::endl;
#endif

	/* ------------------------ */

	if (wsv_pipe(pipefd) < 0)
	{
		free(envp);
		wsv_close(m->cgi_fd); /* insert error check */
		wsv_tcp_multiplexing_cgi_clear(m->fd_sets);
		return (WSV_ERROR_500);
	}

	if (wsv_fcntl_int(pipefd[0], F_SETFL, O_NONBLOCK) == -1)
	{
		free(envp);
		wsv_close(m->cgi_fd); /* insert error check */
		wsv_close(pipefd[0]); /* insert error check */
		wsv_close(pipefd[1]); /* insert error check */
		wsv_tcp_multiplexing_cgi_clear(m->fd_sets);
		return (WSV_ERROR_500);
	}

	pid = wsv_fork();
	if (pid < 0)
	{
		free(envp);
		wsv_close(m->cgi_fd); /* insert error check */
		wsv_close(pipefd[0]); /* insert error check */
		wsv_close(pipefd[1]); /* insert error check */
		wsv_tcp_multiplexing_cgi_clear(m->fd_sets);
		return (WSV_ERROR_500);
	}
	else if (pid == 0)
	{
		wsv_close(pipefd[0]); /* insert error check */

		if (wsv_dup2(m->cgi_fd, STDIN_FILENO) < 0)
		{
			wsv_close(m->cgi_fd); /* insert error check */
			wsv_close(pipefd[1]); /* insert error check */
			exit(EXIT_FAILURE); /* memory leaks */
		}
		if (wsv_dup2(pipefd[1], STDOUT_FILENO) < 0)
		{
			wsv_close(m->cgi_fd); /* insert error check */
			wsv_close(pipefd[1]); /* insert error check */
			exit(EXIT_FAILURE); /* memory leaks */
		}

		if (wsv_execve(m->cgi_bin, argv, envp) < 0)
		{
			free(envp);
			wsv_close(m->cgi_fd); /* insert error check */
			wsv_close(pipefd[1]); /* insert error check */
			exit(EXIT_FAILURE); /* memory leaks */
		}
	}
	else
	{
		free(envp);
		wsv_close(m->cgi_fd); /* insert error check */
		wsv_close(pipefd[1]); /* insert error check */
		m->cgi_pid = pid;
	}

	m->tmp_fd = pipefd[0];
	wsv_client_static_content_read_intialize(pipefd[0], (void*)m);
	m->read_static_content_handler = &wsv_http_cgi_read;

	return (WSV_RETRY);
}

int
wsv_http_cgi(void* http_message)
{
	static const char*			cgi_request_method[4] =
	{
		0,
		"GET",
		"HEAD",
		"POST"
	};
	static const char*			meta_variables[16] =
	{
		"GATEWAY_INTERFACE=CGI/1.1",
		"SERVER_PROTOCOL=HTTP/1.1",
		"SERVER_SOFTWARE=webserv/draft",

		"REMOTE_ADDR=",			/* client ip address */
		"SERVER_NAME=",			/* server ipv6 address */
		"SERVER_PORT=",			/* server port */
		"REQUEST_METHOD=",		/* [1] "GET", [2] HEAD", [3] POST */

		"PATH_INFO=",			/* trailing path after file name */
		"PATH_TRANSLATED=",		/* target ressource of path_info*/
		"REQUEST_URI=",			/* not in rfc cgi/1.1; php: uri->abs_path */
		"SCRIPT_NAME=",			/* target ressource without path_info */
		"QUERY_STRING=",		/* ?query */

		"AUTH_TYPE=",			/* header[8] -> Authorization, 1st token */
		"REMOTE_USER=",			/* header[8] -> Authorization, 2nd token */
		"CONTENT_TYPE=",		/* header[9] -> Content-Type */

		"CONTENT_LENGTH=",		/* message body in octets */
	};
	std::vector<std::string>	variables(meta_variables, meta_variables + 16);
	struct wsv_http_message_s*	m;
	char*						first;
	char*						last;
	char						cwd_buf[PATH_MAX];
	std::string					absolute_file_name;

	m = (struct wsv_http_message_s*)http_message;

	try {
		absolute_file_name = m->cgi_target_ressource;
		if (*(absolute_file_name.rbegin()) != '/')
		{
			if (wsv_getcwd(cwd_buf, PATH_MAX) == 0)
			{
				m->cgi_headers.clear();
				wsv_close(m->cgi_fd); /* insert error check */
				wsv_tcp_multiplexing_cgi_clear(m->fd_sets);
				return (WSV_ERROR_500);
			}
			first = cwd_buf;
			last = cwd_buf;
			while (*last != '\0')
				++last;
			if (first != last && last[-1] != '/')
			{
				*last = '/';
				++last;
			}
			absolute_file_name.insert(0, first, (last - first));
		}
		m->cgi_target_ressource = absolute_file_name.c_str();

		variables.insert(variables.end(),
						 m->cgi_headers.begin(),
						 m->cgi_headers.end());
		m->cgi_headers.clear();

		variables[3].append(m->client_ipv6_addr_str);
		variables[4].append(((wsv_server_s*)m->server)->ipv6_addr_str);
		variables[5].append(((wsv_server_s*)m->server)->port_str);
		variables[6].append(cgi_request_method[m->method]);

		variables[7].append(m->absolute_path);
		variables[8].append(m->cgi_target_ressource);
		variables[9].append(m->absolute_path);
		variables[10].append(m->cgi_target_ressource);
		variables[11].append(m->query_begin, m->query_end);


		first = m->headers[AUTHORIZATION];
		while (*first != '\0' && *first == ' ')
			++first;
		last = first;
		while (*last != '\0' && *last != ' ')
			++last;
		variables[12].append(first, last);
		first = last;
		while (*first != '\0' && *first == ' ')
			++first;
		last = first;
		while (*last != '\0' && *last != ' ')
			++last;
		variables[13].append(first, last);
		first = m->headers[CONTENT_TYPE];
		while (*first != '\0' && *first == ' ')
			++first;
		last = first;
		while (*last != '\0' && *last != ' ')
			++last;
		variables[14].append(first, last);

		do
		{
			variables[15].insert(15, 1, (char)('0' + (m->content_length % 10)));
			m->content_length /= 10;
		} while (m->content_length != 0);

		return (wsv_http_cgi_exec(m, variables));
	}
	catch (const std::exception& e) {
		std::cerr << "\033[0;31m"
				  << "wsv_http_cgi(): "
				  << e.what()
				  << "\033[0;0m"
				  << std::endl;
		m->cgi_headers.clear();
		wsv_close(m->cgi_fd); /* insert error check */
		wsv_tcp_multiplexing_cgi_clear(m->fd_sets);
		return (WSV_ERROR_500);
	}
}
