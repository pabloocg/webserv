#include <iostream>

#include <stdint.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_location.h"
#include "wsv_http_method.h"
#include "wsv_string.h"
#include "wsv_mime_type.h"
#include "wsv_http_cgi.h"
#include "wsv_error_log.h"
#include "wsv_tcp.h"

#define FLAGS		(O_RDWR | O_CREAT | O_TRUNC | O_NOFOLLOW | O_NONBLOCK)
#define MODE		(00666)

int
wsv_http_cgi_get(void* http_message, const void* location)
{
	struct wsv_http_message_s*		m;
	const struct wsv_location_s*	lc;
	char*							target_ressource;
	int								fd;
	struct stat						statbuf;
	std::string						file_tmp;
	uintptr_t						file_tmp_id;

	m = (struct wsv_http_message_s*)http_message;
	lc = (const struct wsv_location_s*)location;

	target_ressource = wsv_http_method_target_ressource(m->absolute_path,
														m->abs_path_root,
														&m->abs_path_extension,
														lc->pattern,
														lc->root);

	if (m->abs_path_extension[0] == '\0' && m->abs_path_extension[-1] == '/')
	{
		if (lc->index != 0)
		{
			fd = 0;
			while ((m->abs_path_extension[fd] = lc->index[fd]) != '\0')
				++fd;
		}

		if (stat(target_ressource, &statbuf) < 0)
			return (WSV_ERROR_404);

		if (S_ISDIR(statbuf.st_mode))
			return (WSV_ERROR_404);

		wsv_str_extension_find(&m->abs_path_extension, target_ressource);
	}
	else
	{
		if (wsv_stat(target_ressource, &statbuf) < 0)
			return (WSV_ERROR_404);
		if (S_ISDIR(statbuf.st_mode))
			return (WSV_ERROR_404);
	}

	/* ++++ CGI parameters +++ */
	try {
		file_tmp_id = (uintptr_t)m;
		do
		{
			file_tmp.insert(0, 1, (char)('0' + (file_tmp_id % 10)));
			file_tmp_id /= 10;
		} while (file_tmp_id != 0);
		file_tmp.insert(0, "/tmp/wsv_cgi_");
		file_tmp.append(".tmp");
	}
	catch (const std::exception& e) {
		std::cerr << "\033[0;31m"
				  << "wsv_http_cgi_get(): "
				  << e.what()
				  << "\033[0;0m"
				  << std::endl;
		return (WSV_ERROR_500);
	}

	m->cgi_bin = lc->cgi_bin;
	m->cgi_target_ressource = target_ressource;
	m->cgi_fd = wsv_open_m(file_tmp.c_str(), FLAGS, MODE);
	if (m->cgi_fd < 0)
		return (WSV_ERROR_500);
	/* +++++++++++++++++++++++ */

	wsv_tcp_multiplexing_clear(m->sockfd, m->fd_sets);
	wsv_tcp_multiplexing_cgi_insert(m->sockfd, m->fd_sets);
	return (WSV_RETRY);
}
