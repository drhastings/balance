#ifndef COORD_H
#define COORD_H

#include <stdint.h>

struct coord
{
	float x;
	float y;

	struct coord *next;

	int on_heap;
};

struct coord *new_coord(int x, int y);

void free_coord(struct coord *coord);

#endif
