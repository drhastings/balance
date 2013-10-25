#include "../includes/coord.h"
#include <stdlib.h>

struct coord *new_coord(int x, int y)
{
	struct coord *ret = (struct coord *)malloc(sizeof(*ret));

	if (ret)
	{
		ret->x = x;
		ret->y = y;

		ret->next = NULL;
	}

	return ret;
}

void free_coord(struct coord *coord)
{
	free(coord);
}
