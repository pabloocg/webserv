#ifndef WSV_LOCATION_H
# define WSV_LOCATION_H

# include <sys/types.h>

struct							wsv_location_s
{
	const char*					pattern;

	const char*					root;
	const char*					index;
	const char*					cgi_bin;
	const char*					cgi_pass;
	int							autoindex;
	int							limit_except;
	size_t						client_max_body_size;
};

int		wsv_locations_initialize(void* lst_locations,
								 void** location_buckets,
								 unsigned int *location_buckets_size);
void	wsv_locations_clear(void* location_buckets,
							unsigned int location_buckets_size);
int		wsv_location(void* location_buckets,
					 const unsigned int location_buckets_size,
					 const char* pattern,
					 const struct wsv_location_s** location);
int		wsv_location2(void* location_buckets,
			 const unsigned int location_buckets_size,
			 const char* pattern,
			 const struct wsv_location_s** location);

#endif
