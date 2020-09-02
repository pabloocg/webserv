#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_linked_list.h"
#include "wsv_string.h"
#include "wsv_hash.h"
#include "wsv_location.h"

static void*
wsv_location_buckets_initialize(unsigned int n_bucket)
{
	void*					buckets;

	n_bucket = (unsigned int)(n_bucket * sizeof(struct wsv_pair_s));

	buckets = wsv_malloc(n_bucket);
	if (buckets != 0)
		wsv_bzero(buckets, n_bucket);

	return (buckets);
}

int
wsv_locations_initialize(void* lst_locations,
						 void** location_buckets,
						 unsigned int *location_buckets_size)
{
	unsigned int			n_bucket;
	struct wsv_pair_s*		buckets;
	struct wsv_location_s*	location;

	wsv_list_size(lst_locations, &n_bucket);
	if (n_bucket == 0)
	{
		*location_buckets = 0;
		*location_buckets_size = 0;
		wsv_list_clear(lst_locations, 0);
		return (WSV_OK);
	}
	n_bucket = n_bucket << 1;

	buckets = (struct wsv_pair_s*)wsv_location_buckets_initialize(n_bucket);
	if (buckets == 0)
		return (WSV_ERROR);

	wsv_list_iterate_initialize(lst_locations);
	while (wsv_list_iterate(lst_locations, (void**)&location) == WSV_RETRY)
	{
		wsv_hash_str_add(buckets,
						 n_bucket,
						 (const void*)(location->pattern),
						 (const void*)location);
	}
	wsv_list_clear(lst_locations, 0);

	*location_buckets = buckets;
	*location_buckets_size = n_bucket;
	return (WSV_OK);
}

void
wsv_locations_clear(void* location_buckets, unsigned int location_buckets_size)
{
	struct wsv_pair_s*		buckets;

	buckets = (struct wsv_pair_s*)location_buckets;
	while (location_buckets_size--)
	{
		free((void*)(buckets[location_buckets_size].value));
	}
	free(location_buckets);
}

int
wsv_location(void* location_buckets,
			 const unsigned int location_buckets_size,
			 const char* pattern,
			 const struct wsv_location_s** location)
{
	if (location_buckets_size == 0)
		return (WSV_ERROR);
	else
		return (wsv_hash_str_find((struct wsv_pair_s*)location_buckets,
								  location_buckets_size,
								  pattern,
								  (const void**)location));
}
