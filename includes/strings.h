#ifndef STRINGS__H
#define STRINGS__H

#include <stdlib.h>

// Add macros for accesing methods

struct string
{
	char * buffer;

	int length;

	int free;

	struct variable *variable;
}; 

// Add class methods here

struct string *new_string(const char *buffer);

struct string *new_string_v(void *buffer, int length);

int string_length(struct string *string);

int send_string(int socketfd, struct string * string, int * counter);

void free_string(struct string *string);

void print_string(struct string *string);

#endif
