#ifndef WSV_HASH_H
# define WSV_HASH_H

/*
** Hash function: linear probing
*/

struct wsv_pair_s
{
	const void*			key;
	const void*			value;
};

int		wsv_hash_str_initialize(struct wsv_pair_s* buckets,
								const unsigned int n_bucket,
								struct wsv_pair_s* elems,
								unsigned int n_elem);

int		wsv_hash_str_add(struct wsv_pair_s* buckets,
						 const unsigned int n_bucket,
						 const void* key,
						 const void* value);

int		wsv_hash_str_find(struct wsv_pair_s* buckets,
						  const unsigned int n_bucket,
						  const void* key,
						  const void** value);

#endif
