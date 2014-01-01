#ifndef QUATERNION_H
#define QUATERNION_H

#include <stdint.h>
#include "vector.h"

struct robot;

struct quaternion
{
	float w;
	float x;
	float y;
	float z;

	struct vect gravity;
};

int getQuaternion(struct quaternion *output, char *packet);

float get_tilt_angle(struct robot *robot);

float quaternion_to_tilt(struct quaternion *quat);

float quaternion_to_theta(struct quaternion *quat);

void quaternion_product(struct quaternion *result, struct quaternion * p,
			struct quaternion *q);

void rotate_vector(struct vect *out, struct vect *in, struct quaternion *r);

void get_conjugate(struct quaternion *result, struct quaternion *q);

#endif
