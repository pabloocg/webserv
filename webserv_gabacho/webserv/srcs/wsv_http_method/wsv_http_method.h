#ifndef WSV_HTTP_METHOD_H
# define WSV_HTTP_METHOD_H

# define HTTP_METHOD_UNKNOWN			0
# define HTTP_METHOD_GET				1
# define HTTP_METHOD_HEAD				2
# define HTTP_METHOD_POST				3
# define HTTP_METHOD_PUT				4
# define HTTP_METHOD_DELETE				5
# define HTTP_METHOD_CONNECT			6
# define HTTP_METHOD_OPTIONS			7
# define HTTP_METHOD_TRACE				8

char*	wsv_http_method_target_ressource(char* absolute_path,
										 char* abs_path_root,
										 char** abs_path_extension,
										 const char* location_pattern,
										 const char* location_root);

#endif
