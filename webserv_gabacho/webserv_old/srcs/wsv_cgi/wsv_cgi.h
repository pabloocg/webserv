#ifndef WSV_CGI_H
# define WSV_CGI_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <string>
#include <list>
#include <sys/stat.h>
#include <fcntl.h>
#include "wsv_retval.h"
#include "wsv_http_header.h"
#include "wsv_wrapper.h"

int		wsv_cgi(char headers[HEADERS_MAX][HEADER_VALUE_SIZE],
				char *clientIp,
				char *pathToCgiExecutable,
				char *requestMethod,
				std::string &requestUri,
				char *scriptName,
				char *queryString,
				int cgi_input,
				void* m);

int		wsv_http_cgi_read(int fd, struct wsv_http_message_s* m);

#endif
