#ifndef PAGE__H
#define PAGE__H

#include <stdlib.h>
#include "../includes/list.h"
#include "../includes/server.h"

// Add macros for accesing methods

struct page
{
	struct string *name;

	struct node *root;

	struct list_head list_item;

	struct list_head variables;
};

// Add class methods here

struct page *new_page(const char *name, struct node *node,
												struct server *server);

void free_page(struct page *page);

#endif
