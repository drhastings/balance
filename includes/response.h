#ifndef RESPONSE__H
#define RESPONSE__H

#include <stdlib.h>
#include "../includes/list.h"
#include "../includes/first_line.h"
#include "../includes/http_header.h"

// Add macros for accesing methods

struct response
{
	struct status_line *status_line;

	struct list_head responses;
};

// Add class methods here

struct response *new_response_v(struct status_line *first_line,
													struct http_header *header);

void free_response(struct response *response);

int send_response(int sockfd, struct response *response, int *counter);

void print_response(struct response *response);

struct response *parse_response(char *buffer, int length);

#endif
