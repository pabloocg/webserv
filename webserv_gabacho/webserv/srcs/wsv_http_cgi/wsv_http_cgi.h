#ifndef WSV_HTTP_CGI_H
# define WSV_HTTP_CGI_H

int		wsv_http_cgi(void* http_message);

int		wsv_http_cgi_get(void* http_message, const void* location);
int		wsv_http_cgi_post(void* http_message, const void* location);

#endif
