#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>

struct vect
{
	float x;
	float y;
	float z;
};

void find_floor_intersection(struct vect *out, struct vect *point,
                              struct vect * slope);

void cross(struct vect *a, struct vect *b, struct vect * ab);

float norm(struct vect *a);

float dot(struct vect *a, struct vect *b);

float distance_on_ground(struct vect *a, struct vect *b);

void difference(struct vect *out, struct vect *a, struct vect *b);

float normalized_angle(float a);

#endif
