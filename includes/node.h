#ifndef NODE__H
#define NODE__H

#include <stdlib.h>
#include "../includes/list.h"
#include "../includes/property.h"

// Add macros for accesing methods

struct node
{
	struct list_head list_item;

	int (*send)(int, struct node *, int *);

	void (*free)(struct node *);

	int (*length)(struct node*);

	struct string *type;

	struct list_head properties;

	struct node *parent;

	struct list_head children;
};

// Add class methods here

struct node *new_node(const char *type,
													struct property *property, struct node *parent);

struct node *new_node_v(struct string *type,
													struct property *property, struct node *parent);

void free_node(struct node *node);

void print_node(struct node *node);

int send_node(int socketfd, struct node *node, int *counter);

int node_length(struct node *node);

struct node *parse_node(char *buffer, int length);

#endif
