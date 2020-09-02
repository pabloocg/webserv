#include <stdint.h>

#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_hash.h"
#include "wsv_string.h"

static uint32_t
wsv_hash_str_fnv_1a(const char* str)
{
	uint32_t			hash;

	hash = 0x811c9dc5;
	while (*str != '\0')
	{
		hash ^= (uint32_t)*str;
		hash += (hash<<1) + (hash<<4) + (hash<<7) + (hash<<8) + (hash<<24);
		++str;
	}

	return (hash);
}

int
wsv_hash_str_initialize(struct wsv_pair_s* buckets,
						const unsigned int n_bucket,
						struct wsv_pair_s* elems,
						unsigned int n_elem)
{
	uint32_t			hash_code;

	if (n_bucket < n_elem)
		return (WSV_ERROR);
	else
	{
		wsv_bzero((void*)buckets, n_bucket * sizeof(struct wsv_pair_s));
		while (n_elem--)
		{
			hash_code = wsv_hash_str_fnv_1a((const char*)elems[n_elem].key);

			hash_code %= n_bucket;
			while (buckets[hash_code].key != 0)
				hash_code = (hash_code + 1) == n_bucket ? 0 : hash_code + 1;

			buckets[hash_code].key = elems[n_elem].key;
			buckets[hash_code].value = elems[n_elem].value;
		}

		return (WSV_OK);
	}
}

int
wsv_hash_str_add(struct wsv_pair_s* buckets,
				 const unsigned int n_bucket,
				 const void* key,
				 const void* value)
{
	uint32_t			hash_code;
	int					is_full;

	hash_code = wsv_hash_str_fnv_1a((const char*)key);

	hash_code %= n_bucket;
	is_full = 0;
	while (buckets[hash_code].key != 0)
	{
		if ((hash_code + 1) == n_bucket)
		{
			hash_code = 0;
			if (++is_full == 2)
				return (WSV_ERROR);
		}
		else
			++hash_code;
	}

	buckets[hash_code].key = key;
	buckets[hash_code].value = value;
	return (WSV_OK);
}

int
wsv_hash_str_find(struct wsv_pair_s* buckets,
				  const unsigned int n_bucket,
				  const void* key,
				  const void** value)
{
	uint32_t			hash_code;

	hash_code = wsv_hash_str_fnv_1a((const char*)key);
	hash_code %= n_bucket;

	while (buckets[hash_code].key != 0 &&
		   wsv_strcmp((const char*)buckets[hash_code].key,
					  (const char*)key) != 0)
		hash_code = (hash_code + 1) == n_bucket ? 0 : hash_code + 1;

	if (buckets[hash_code].key != 0)
	{
		*value = buckets[hash_code].value;
		return (WSV_OK);
	}
	else
		return (WSV_ERROR);
}
