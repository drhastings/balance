#ifndef CHILD__H
#define CHILD__H

#include "../includes/list.h"

struct child
{
	struct list_head list_item;

	int (*send)(int, void *, int *);

	void (*free)(void *);

	int (*length)(void *);
};

#endif
