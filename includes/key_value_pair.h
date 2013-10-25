#ifndef KEY_VALUE_PAIR__H
#define KEY_VALUE_PAIR__H

#include <stdlib.h>
#include "../includes/list.h"
#include "../includes/strings.h"

// Add macros for accesing methods

struct key_value_pair {
	// Add members here
	struct string * key;

	struct string * separator;

	struct string * value;

	struct string * end;

	int length;

	struct list_head list_item;
	// Add methods here
};

// Add class methods here

struct key_value_pair *new_key_value_pair(const char *key,
													const char *separator, const char *value,
														const char *end, struct key_value_pair * pair);

struct key_value_pair *new_key_value_pair_v(struct string *key,
													struct string *separator,
														struct string *value,
														struct string *end, struct key_value_pair *pair);

int append_to_pair(struct key_value_pair *tail, struct key_value_pair *head);

int key_value_pair_length(struct key_value_pair *pair);

int send_key_value_pair(int socketfd, struct key_value_pair *pair,
													int *counter);

void free_key_value_pair(struct key_value_pair *key_value_pair);

void free_key_value_pairs(struct key_value_pair *key_value_pair);

void print_key_value_pair(struct key_value_pair *key_value_pair);

void print_key(struct key_value_pair *key_value_pair);

void print_value(struct key_value_pair *key_value_pair);

struct key_value_pair *parse_key_value_pair(const char *separator,
														const char *end, char *buffer, int length);

#endif
