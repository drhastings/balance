#ifndef PROPERTY__H
#define PROPERTY__H

#include <stdlib.h>
#include "../includes/list.h"

// Add macros for accesing methods

struct property {
	// Add members here
	struct string * name;

	struct string * separator;

	struct string * value;

	struct string * end;

	int length;

	struct list_head list_item;
	// Add methods here
};

// Add class methods here

struct property *new_property(const char *name, const char *value,
															struct property *property);

struct property *new_property_v(struct string *key,
													struct string *separator,
														struct string *value,
														struct string *end, int length);

inline int append_to_property(struct property *tail, struct property *head);

void free_property(struct property *property);

void print_property(struct property *property);

int property_length(struct property *property);

int send_property(int socketfd, struct property *property, int *counter);

struct property *parse_property(char *buffer, int length);

#endif
