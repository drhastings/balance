#ifndef STATUS_LINE__H
#define STATUS_LINE__H

#include <stdlib.h>
#include "../includes/list.h"
#include "../includes/strings.h"

// Add macros for accesing methods

struct status_line {
	// Add members here
	struct string * protocol;

	struct string * status_code;

	struct string * reason;

	int length;
};

// Add class methods here

struct status_line *new_status_line(const char *protocol,
													const char *status_code,
														const char *reason);

struct status_line *new_status_line_v(struct string *protocol,
													struct string *status_code,
														struct string *reason);

int status_line_length(struct status_line * status_line);

int send_status_line(int socketfd, struct status_line *status_line,
											int *counter);

void free_status_line(struct status_line *status_line);

void print_status_line(struct status_line *status_line);

struct status_line *parse_status_line(char *buffer, int length);

#endif
