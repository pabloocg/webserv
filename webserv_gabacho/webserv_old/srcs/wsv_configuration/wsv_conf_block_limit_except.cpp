#include "wsv_retval.h"
#include "wsv_configuration.h"
#include "wsv_string.h"
#include "wsv_http_method.h"
#include "wsv_location.h"
#include "wsv_linked_list.h"

int
wsv_conf_limit_except_block(struct wsv_server_s* srv,
							struct wsv_conf_data_s* cd)
{
	struct wsv_location_s*	location;
	char*					token;

	if ((cd->key_end - cd->key_begin) != 12 ||
		!wsv_str_cmp_12(cd->key_begin, "limit_except") ||
		cd->value_begin == 0)
	{
		return (WSV_ERROR);
	}

	wsv_list_data_front(srv->lst_locations, (void**)&location);

	do
	{
		token = wsv_str_token(cd->value_begin,
							  cd->value_end,
							  &cd->value_begin,
							  ' ');
		if (wsv_strcmp(token, "GET") == 0)
			location->limit_except += (1 << HTTP_METHOD_GET);
		else if (wsv_strcmp(token, "PUT") == 0)
			location->limit_except += (1 << HTTP_METHOD_PUT);
		else if (wsv_strcmp(token, "HEAD") == 0)
			location->limit_except += (1 << HTTP_METHOD_HEAD);
		else if (wsv_strcmp(token, "POST") == 0)
			location->limit_except += (1 << HTTP_METHOD_POST);
		else if (wsv_strcmp(token, "TRACE") == 0)
			location->limit_except += (1 << HTTP_METHOD_TRACE);
		else if (wsv_strcmp(token, "DELETE") == 0)
			location->limit_except += (1 << HTTP_METHOD_DELETE);
		else if (wsv_strcmp(token, "OPTIONS") == 0)
			location->limit_except += (1 << HTTP_METHOD_OPTIONS);
		else if (wsv_strcmp(token, "CONNECT") == 0)
			location->limit_except += (1 << HTTP_METHOD_CONNECT);
		else
			return (WSV_ERROR);
	} while (cd->value_begin < cd->value_end);

	cd->context = block_limit_execept;
	return (WSV_OK);
}

int
wsv_conf_limit_except_directive(struct wsv_server_s* srv,
								struct wsv_conf_data_s* cd)
{
	const char*		value;

	(void)srv;

	if ((cd->key_end - cd->key_begin) == 4 &&
		wsv_str_cmp_4(cd->key_begin, "deny") &&
		cd->value_begin != 0)
	{
		wsv_conf_directive_string(cd->value_begin,
								  cd->value_end,
								  &value);
		if (wsv_strcmp(value, "all") != 0)
			return (WSV_ERROR);

		return (WSV_OK);
	}
	else
		return (WSV_ERROR);
}
