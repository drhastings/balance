#ifndef REQUEST__H
#define REQUEST__H

#include <stdlib.h>
#include "../includes/list.h"
#include "../includes/http_header_request.h"
#include "../includes/http_header.h"

// Add macros for accesing methods

struct request
{
	struct http_header_request *request;

	struct list_head headers;
};

// Add class methods here

struct request *new_http_request_v(struct http_header_request * request,
													struct http_header * header);

int request_length(struct request *request);

void free_request(struct request *request);

void print_request(struct request *request);

struct request *parse_request(char *buffer, int length);

#endif
