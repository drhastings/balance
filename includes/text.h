#ifndef TEXT__H
#define TEXT__H

#include <stdlib.h>
#include "../includes/list.h"

// Add macros for accesing methods

struct text
{
	struct list_head list_item;

	int (*send)(int, struct text *, int *);

	void (*free)(struct text *);

	int (*length)(struct text*);

	struct string *contents;

	struct node *parent;
};

// Add class methods here

struct text *new_text(const char *contents, struct node *parent);

struct text *new_text_v(struct string *contents, struct node *parent);

void free_text(struct text *text);

void print_text(struct text *text);

int text_length(struct text *text);

int send_text(int socketfd, struct text *text, int *counter);

struct text *parse_text(char *buffer, int length);

#endif
