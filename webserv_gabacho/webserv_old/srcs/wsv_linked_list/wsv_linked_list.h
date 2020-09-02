#ifndef WSV_LINKED_LIST_H
# define WSV_LINKED_LIST_H

struct						wsv_node_s
{
	void*					data;
	struct wsv_node_s*		next;
};

struct						wsv_linked_list_s
{
	struct wsv_node_s*		head;
	struct wsv_node_s*		iterator;
	unsigned int			n_elems;
};

int		wsv_list_initialize(void** lst);
int		wsv_list_push_front(void* lst, void* data);
void	wsv_list_clear(void* lst, void (*clear)(void*));
void	wsv_list_data_front(void* lst, void** data);
void	wsv_list_size(void* lst, unsigned int* size);
void	wsv_list_iterate_initialize(void* lst);
int		wsv_list_iterate(void* lst, void** data);

#endif
