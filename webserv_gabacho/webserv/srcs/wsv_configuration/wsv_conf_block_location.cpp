#include <limits.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_configuration.h"
#include "wsv_string.h"
#include "wsv_location.h"
#include "wsv_linked_list.h"

#define LOCATION_SIZE	sizeof(struct wsv_location_s)

int
wsv_conf_location_block(struct wsv_server_s* srv,
						struct wsv_conf_data_s* cd)
{
	struct wsv_location_s*	location;

	if ((cd->key_end - cd->key_begin) != 8 ||
		!wsv_str_cmp_8(cd->key_begin, "location") ||
		cd->value_begin == 0)
	{
		return (WSV_ERROR);
	}

	location = (struct wsv_location_s*)wsv_malloc(LOCATION_SIZE);
	if (location == 0)
		return (WSV_ERROR);

	wsv_conf_directive_string(cd->value_begin,
							  cd->value_end,
							  &location->pattern);
	location->root = 0;
	location->index = 0;
	location->cgi_bin = 0;
	location->cgi_pass = 0;
	location->autoindex = 0;
	location->limit_except = 0;
	location->client_max_body_size = 0;

	if (wsv_list_push_front(srv->lst_locations, location) == WSV_ERROR)
	{
		free(location);
		return (WSV_ERROR);
	}
	else
	{
		cd->context = block_location;
		return (WSV_OK);
	}
}

int
wsv_conf_location_directive(struct wsv_server_s* srv,
							struct wsv_conf_data_s* cd)
{
	struct wsv_location_s*	location;
	const char*				value;

	if (cd->value_begin == 0)
		return (WSV_ERROR);

	wsv_list_data_front(srv->lst_locations, (void**)&location);

	switch (cd->key_end - cd->key_begin)
	{
	case 4:
		if (wsv_str_cmp_4(cd->key_begin, "root"))
		{
			if ((cd->value_end - cd->value_begin) > (PATH_MAX >> 1))
				return (WSV_ERROR);
			return (wsv_conf_directive_string(cd->value_begin,
											  cd->value_end,
											  &location->root));
		}
		break;
	case 5:
		if (wsv_str_cmp_5(cd->key_begin, "index"))
		{
			if ((cd->value_end - cd->value_begin) > NAME_MAX)
				return (WSV_ERROR);
			return (wsv_conf_directive_string(cd->value_begin,
											  cd->value_end,
											  &location->index));
		}
		break;
	case 7:
		if (wsv_str_cmp_7(cd->key_begin, "cgi_bin"))
		{
			return (wsv_conf_directive_string(cd->value_begin,
											  cd->value_end,
											  &location->cgi_bin));
		}
	case 8:
		if (wsv_str_cmp_8(cd->key_begin, "cgi_pass"))
		{
			return (wsv_conf_directive_string(cd->value_begin,
											  cd->value_end,
											  &location->cgi_pass));
		}
	case 9:
		if (wsv_str_cmp_9(cd->key_begin, "autoindex"))
		{
			wsv_conf_directive_string(cd->value_begin,
									  cd->value_end,
									  &value);
			if (wsv_strcmp(value, "on") == 0)
				location->autoindex = 1;
			else if (wsv_strcmp(value, "off") == 0)
				location->autoindex = 0;
			else
				return (WSV_ERROR);
			return (WSV_OK);
		}
	case 20:
		if (wsv_str_cmp_20(cd->key_begin, "client_max_body_size"))
		{
			return (wsv_conf_directive_size(cd->value_begin,
											cd->value_end,
											&location->client_max_body_size));
		}
	}

	return (WSV_ERROR);
}
