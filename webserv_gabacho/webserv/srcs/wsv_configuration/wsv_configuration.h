#ifndef WSV_CONFIGURATION_H
# define WSV_CONFIGURATION_H

# include <stddef.h>

enum g_wsv_context
{
	block_none = 0,
	block_server,
	block_location,
	block_limit_execept
};

struct							wsv_conf_data_s
{
	int							state;
	int							context;

	char*						buf_begin;
	char*						buf_end;
	char*						buf_position;

	char*						key_begin;
	char*						key_end;

	char*						value_begin;
	char*						value_end;

	int							nblock;
};

struct							wsv_server_s
{
	int							listen_sockfd;
	int							duplicate;

	unsigned char				ipv6_addr[16];
	unsigned char				port[2];
	const char*					server_name;
	const char*					error_pages[14];

	/* location */
	void*						lst_locations;
	void*						location_buckets;
	unsigned int				location_buckets_size;

	/* add-on */
	char						ipv6_addr_str[40];
	char						port_str[6];
};

struct							wsv_configuration_s
{
	char*						buf;

	struct wsv_server_s*		servers;
	int							n_server;

	struct wsv_server_s*		srv; /* current server */
};

/*
** ==========
** = public =
** ==========
*/
int		wsv_configuaration(const char* pathname,
						   struct wsv_configuration_s* conf);

/*
** ===========
** = private =
** ===========
*/
int		wsv_conf_read(const char* pathname,
					  struct wsv_configuration_s* conf,
					  struct wsv_conf_data_s* cd);
int		wsv_conf_parse(struct wsv_conf_data_s* conf);
int		wsv_conf_process(struct wsv_configuration_s* conf,
						 struct wsv_conf_data_s* cd);

/*
** context/block: server
*/
int		wsv_conf_server_block(struct wsv_configuration_s* conf,
							  struct wsv_conf_data_s* cd);
int		wsv_conf_server_directive(struct wsv_server_s* srv,
								  struct wsv_conf_data_s* cd);

/*
** context/block: location
*/
int		wsv_conf_location_block(struct wsv_server_s* srv,
								struct wsv_conf_data_s* cd);
int		wsv_conf_location_directive(struct wsv_server_s* srv,
									struct wsv_conf_data_s* cd);

/*
** context/block: limit except
*/
int		wsv_conf_limit_except_block(struct wsv_server_s* srv,
									struct wsv_conf_data_s* cd);
int		wsv_conf_limit_except_directive(struct wsv_server_s* srv,
										struct wsv_conf_data_s* cd);

/*
** directives
*/
int		wsv_conf_directive_error_page(char* value_begin,
									  char* value_end,
									  const char** error_pages);
int		wsv_conf_directive_listen(char* value_begin,
								  char* value_end,
								  unsigned char ipv6_addr[16],
								  unsigned char port[2]);
int		wsv_conf_directive_size(char* first, char* last, size_t* dst);
int		wsv_conf_directive_string(char* first, char* last, const char** dst);

#endif
