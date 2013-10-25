#ifndef HTTP_HEADER__H
#define HTTP_HEADER__H

#include <stdlib.h>
#include "../includes/list.h"

// Add macros for accesing methods

struct http_header {
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

struct http_header *new_http_header(const char *name, const char *value,
																			struct http_header *header);

struct http_header *new_http_header_v(struct string *key,
													struct string *separator,
														struct string *value,
														struct string *end, int length);

int http_header_length(struct http_header * header);

int send_http_header(int socketfd, struct http_header *http_header,
											int *counter);

void free_http_header(struct http_header *http_header);

void print_http_header(struct http_header *http_header);

struct http_header *parse_http_header(char *buffer, int length);

#endif
