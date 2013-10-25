#ifndef HTTP_HEADER_REQUEST__H
#define HTTP_HEADER_REQUEST__H

#include <stdlib.h>
#include "../includes/list.h"
#include "../includes/strings.h"

// Add macros for accesing methods

struct http_header_request {
	// Add members here
	struct string * method;

	struct string * uri;

	struct string * protocol;

	int length;
};

// Add class methods here

struct http_header_request *new_http_header_request(const char *method,
													const char *uri,
														const char *protocol);

struct http_header_request *new_http_header_request_v(struct string *method,
													struct string *uri,
														struct string *protocol);

int http_header_request_length(struct http_header_request *request);

int send_http_header_request(int socketfd, struct http_header_request *request,
															int *counter);

void free_http_header_request(struct http_header_request *http_header_request);

void print_http_header_request(struct http_header_request *http_header_request);

struct http_header_request *parse_http_header_request(char *buffer, int length);

#endif
