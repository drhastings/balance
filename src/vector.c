#include "../includes/vector.h"
#include <math.h>

void cross(struct vect *a, struct vect *b, struct vect * ab)
{
	ab->x = a->y * b->z - a->z * b->y;

	ab->y = a->z * b->x - a->x * b->z;

	ab->z = a->x * b->y - a->y * b->x;
}

float norm(struct vect *a)
{
	return sqrtf(a->x * a->x + a->y * a->y + a->z * a->z);
}

float dot(struct vect *a, struct vect *b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}
