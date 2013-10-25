#ifndef FIRST_LINE__H
#define FIRST_LINE__H

#include <stdlib.h>
#include "../includes/list.h"
#include "../includes/strings.h"

// Add macros for accesing methods

struct first_line {
	// Add members here
	struct string * protocol;

	struct string * status_code;

	struct string * reason;

	int length;
};

int first_line_length(struct first_line *first_line);

#endif
