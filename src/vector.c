#include "../includes/vector.h"
#include <math.h>

void find_floor_intersection(struct vect *out, struct vect *point,
                              struct vect * slope)
{
  float t;

  t = (point->z + 40) / -slope->z;

  out->x = slope->x * t + point->x;
  out->y = slope->y * t + point->y;

  out->z = -40;
}
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

float distance_on_ground(struct vect *a, struct vect *b)
{
	struct vect temp_a, temp_b, temp_c;

	temp_a = *a;
	temp_a.z = 0;

	temp_b = *b;
	temp_b.z = 0;

	difference(&temp_c, &temp_a, &temp_b);

	return norm(&temp_c);
}

float angle_on_ground(struct vect *a, struct vect *b)
{
	struct vect temp_a, temp_b, temp_c;

	temp_a = *a;
	temp_a.z = 0;

	temp_b = *b;
	temp_b.z = 0;

	difference(&temp_c, &temp_a, &temp_b);

	return atan2f(temp_c.x, temp_c.y);
}

float normalized_angle(float a)
{
	while (a > M_PI)
		a -= 2 * M_PI;

	while (a < -M_PI)
		a += 2 * M_PI;

	return a;
}

void difference(struct vect *out, struct vect *a, struct vect *b)
{
	out->x = a->x - b->x;
	out->y = a->y - b->y;
	out->z = a->z - b->z;
}

