#include <stdlib.h>
#include <limits.h>

#include "wsv_retval.h"
#include "wsv_wrapper.h"
#include "wsv_linked_list.h"

int
wsv_list_initialize(void** lst)
{
	void*						lst_new;

	lst_new = wsv_malloc(sizeof(struct wsv_linked_list_s));
	if (lst_new == 0)
		return (WSV_ERROR);

	((struct wsv_linked_list_s*)lst_new)->head = 0;
	((struct wsv_linked_list_s*)lst_new)->iterator = 0;
	((struct wsv_linked_list_s*)lst_new)->n_elems = 0;

	*lst = lst_new;

	return (WSV_OK);
}

int
wsv_list_push_front(void* lst, void* data)
{
	struct wsv_node_s*			node;

	if (((struct wsv_linked_list_s*)lst)->n_elems == UINT_MAX)
		return (WSV_ERROR);

	node = (struct wsv_node_s*)wsv_malloc(sizeof(struct wsv_node_s));
	if (node == 0)
		return (WSV_ERROR);

	node->data = data;
	node->next = ((struct wsv_linked_list_s*)lst)->head;

	((struct wsv_linked_list_s*)lst)->head = node;
	((struct wsv_linked_list_s*)lst)->n_elems += 1;

	return (WSV_OK);
}

void
wsv_list_clear(void* lst, void (*clear)(void*))
{
	struct wsv_node_s*			node;
	struct wsv_node_s*			tmp;

	if (lst != 0)
	{
		node = ((struct wsv_linked_list_s*)lst)->head;
		if (clear != 0)
		{
			while (node)
			{
				tmp = node;
				node = node->next;
				clear(tmp->data);
				free((void*)tmp);
			}
		}
		else
		{
			while (node)
			{
				tmp = node;
				node = node->next;
				free((void*)tmp);
			}
		}
	}

	free(lst);
}

void
wsv_list_data_front(void* lst, void** data)
{
	*data = ((struct wsv_linked_list_s*)lst)->head->data;
}

void
wsv_list_size(void* lst, unsigned int* size)
{
	*size = ((struct wsv_linked_list_s*)lst)->n_elems;
}

void
wsv_list_iterate_initialize(void* lst)
{
	struct wsv_linked_list_s*	list;

	list = (struct wsv_linked_list_s*)lst;

	list->iterator = list->head;
}

int
wsv_list_iterate(void* lst, void** data)
{
	struct wsv_linked_list_s*	list;

	list = (struct wsv_linked_list_s*)lst;

	if (list->iterator != 0)
	{
		*data = list->iterator->data;
		list->iterator = list->iterator->next;
		return (WSV_RETRY);
	}
	else
	{
		list->iterator = list->head;
		return (WSV_OK);
	}
}
