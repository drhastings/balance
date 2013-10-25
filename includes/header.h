#ifndef HEADER__H
#define HEADER__H

#include <stdlib.h>
#include "../includes/list.h"
#include "../includes/first_line.h"
#include "../includes/http_header.h"

// Add macros for accesing methods

struct header
{
	struct first_line *first_line;

	struct list_head headers;
};

// Add class methods here

struct header *new_header_v(struct first_line *first_line,
													struct http_header *header);

int header_length(struct header *header);

void free_header(struct header *header);

int send_header(int sockfd, struct header *header, int *counter);

void print_header(struct header *header);

struct header *parse_header(char *buffer, int length);

#endif
