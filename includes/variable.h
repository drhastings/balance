#ifndef VARIABLE__H
#define VARIABLE__H

#include <stdlib.h>
#include "../includes/page.h"
#include "../includes/list.h"
#include "../includes/property.h"

// Add macros for accesing methods

struct variable
{
	struct string *name;

	volatile void *value;

	int value_len;

	void *(*value_in)(struct variable *, struct string *);

	struct string *(*value_out)(struct variable *, struct string *);

	struct list_head list_item;
};

// Add class methods here

struct variable *new_variable(struct page *page, const char *name,
																volatile void *value, int value_len,
									void *(*value_in)(struct variable *, struct string *),
			struct string *(*value_out)(struct variable *, struct string *));

void free_variable(struct variable *variable);

int send_variable(int socketfd, struct variable *variable, int *counter);

int update_variable_by_name(struct string *name, struct page *page,
															struct string *string);

#endif
