#include <stdint.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_http_header.h"
#include "wsv_http_special_characters.h"
#include "wsv_mime_type.h"
#include "wsv_string.h"
#include "wsv_location.h"
#include "wsv_http_method.h"
#include "wsv_cgi.h"
#include "wsv_configuration.h"

#define FLAGS		(O_RDWR| O_CREAT | O_TRUNC | O_NOFOLLOW)
#define MODE		(00666)

static const char*	wsv_http_methods_names[] =
{
	0,
	"GET",
	"HEAD",
	"POST",
	"PUT",
	"DELETE",
	"CONNECT",
	"OPTIONS",
	"TRACE"
};

int
wsv_http_method_cgi_no_body(struct wsv_http_message_s* m, const void* location,
							std::string& cgiExecutable)
{
	char*							target_ressource;
	struct stat						statbuf;
	const struct wsv_location_s*	lc;
	std::string						temp;

	m->cgi_executable = cgiExecutable;
	lc = (const struct wsv_location_s*)location;

	wsv_http_method_target_ressource_2(&m->target_ressource,
		m->absolute_path, lc->pattern, lc->root);
	target_ressource = (char*)m->target_ressource.c_str();

	size_t pos = m->target_ressource.find('.');
	if (pos == std::string::npos)
		m->abs_path_extension = (char*)"";
	else
		m->abs_path_extension = (char*)(m->target_ressource.c_str() + pos);

	if (stat(target_ressource, &statbuf) < 0)
		return (WSV_ERROR_404);
	if (S_ISDIR(statbuf.st_mode))
		return (WSV_ERROR_404);

	wsv_mime_type(m->abs_path_extension, &m->content_type);
	temp = std::string(m->absolute_path);
	int i=0;
	int i_begin;
	while(	m->absolute_path_begin[i] != '\0' &&
			m->absolute_path_begin[i] != '?' &&
			m->absolute_path_begin[i] != ' ')
		i++;
	if(m->absolute_path_begin[i] == '?'){
		++i;
		i_begin = i;
		while(	m->absolute_path_begin[i] != '\0' &&
				m->absolute_path_begin[i] != ' ')
			++i;
		m->query = std::string(	&m->absolute_path_begin[i_begin],
								&m->absolute_path_begin[i]);
	}
	else
		m->query = "";

	if ((m->cgi_input = wsv_open_m("cgi_input.tmp", FLAGS, MODE)) < 0)
		return (WSV_ERROR_500);

	return (wsv_cgi(m->headers,
					m->client_ipv6_addr_str,
					(char*)m->cgi_executable.c_str(),
					(char*)wsv_http_methods_names[m->method],
					temp,
					(char*)m->target_ressource.c_str(),
					(char*)m->query.c_str(),
					m->cgi_input,
					m));
}
