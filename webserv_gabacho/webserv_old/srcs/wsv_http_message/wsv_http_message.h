#ifndef WSV_HTTP_REQUEST_H
# define WSV_HTTP_REQUEST_H

# include <sys/types.h>
# include <limits.h>
# include <string>
# include <list>

# include "wsv_http_header.h" /* inclusion dependency */

# define BUF_SIZE					4096 /* minimum 1024 */

# define SCHEME_HTTP				1
# define SCHEME_HTTPS				2
# define HTTP_1_1					1

struct								wsv_http_message_s
{
	int								sockfd;
	const void*						server;
	const void*						configuration;
	void*							fd_sets;

	char							client_ipv6_addr_str[40];
	char							client_port_str[6];

	/* ==== buffer ==== */
	char							buf[BUF_SIZE];

	int								buf_size; /* constant value */
	char*							buf_begin; /* constant valuue */
	char*							buf_end;
	char*							buf_position; /* request only */
	int								buf_count;

	int								state_request;
	int								state_request_line;
	int								state_headers;
	int								state_payload;
	int								state_response;

	/*
	** +++++++++++++++++++++++++++++
	** +          REQUEST          +
	** +++++++++++++++++++++++++++++
	*/

	/* ==== request-line ==== */
	int								method;
	int								scheme;
	char*							host_begin;
	char*							host_end;
	char*							port_begin;
	char*							port_end;
	char*							absolute_path_begin;
	char*							absolute_path_end;
	char*							query_begin;
	char*							query_end;
	int								http_version;

	/* ==== header-field ==== */
	char*							header_name_begin;
	char*							header_name_end;
	char*							header_value_begin;
	char*							header_value_end;

	/* ==== headers ==== */
	char							headers[HEADERS_MAX][HEADER_VALUE_SIZE];
	int								headers_flag;

	/* ==== payload body ==== */
	char*							body_begin;
	char*							body_end;
	size_t							payload_size;

	/* ---- absolute path ---- */
	char*							absolute_path;
	char							abs_path_root[PATH_MAX + NAME_MAX + 1];
	char							abs_path_directory[PATH_MAX >> 1];
	char*							abs_path_extension;

	/* ---- http request read ---- */
	int (*read_request_handler)(int fd, struct wsv_http_message_s* m);
	int (*write_static_content_handler)(int fd, struct wsv_http_message_s* m);
	int (*read_static_content_handler)(int fd, struct wsv_http_message_s* m);

	/*
	** +++++++++++++++++++++++++++++
	** +          RESPONSE         +
	** +++++++++++++++++++++++++++++
	*/
	int								status_code;
	int								ressource_fd;

	const char*						content_type;
	size_t							content_length;
	time_t							last_modified;
	const char*						location;
	const char*						content_location;
	int								limit_except;

	/* ---- cgi ---- */
	std::string						cgi_string;

	std::string						cgi_executable;
	char							buff_post[100];
	std::string						target_ressource;
	std::string						query;
	size_t							cgi_header_length;

	int								cgi_input;
	const char*						cgi_begin;
	const char*						cgi_end;

	std::list<std::string>			cgi_headers;
	ssize_t							cgi_size;

	size_t							max;
};

void	wsv_http_message_initialize(struct wsv_http_message_s* m);
void	wsv_http_message_terminate(struct wsv_http_message_s* m);

/*
** ===========
** = request =
** ===========
*/
int		wsv_http_request_read(int sockfd, struct wsv_http_message_s* m);
int		wsv_http_request_host(struct wsv_http_message_s* m);

int		wsv_http_request_body_read(int sockfd, struct wsv_http_message_s* m);
int		wsv_http_request_body_chunked_read(int sockfd,
										   struct wsv_http_message_s* m);

int		wsv_http_request_body_chunked_parse(struct wsv_http_message_s* m);

/*
** ============
** = response =
** ============
*/
void	wsv_http_response(struct wsv_http_message_s* m);
int		wsv_http_response_write(int sockfd, struct wsv_http_message_s* m);
int		wsv_http_response_ressource_read(int fd, struct wsv_http_message_s* m);

void	wsv_http_response_status_line(int status_code,
									  struct wsv_http_message_s* m);
void	wsv_http_response_headers(struct wsv_http_message_s* m);
void	wsv_http_response_error(struct wsv_http_message_s* m);

#endif
