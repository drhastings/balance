#ifndef POSITION__H
#define POSITION__H

#include <math.h>
#include "quaternion.h"
#include "vector.h"

struct position
{
	int has_traction;

	float linear_count;

	float last_left;

	float last_right;

	float tilt;

	float heading;

	float inertial_heading;

	float last_inertial_heading;

	float incr_inertial_heading;

	float x;

	float y;

	float x_inertial;

	float y_inertial;

	float wheel_base;

	float accel_offset_x;

	float accel_offset_y;

	float accel_offset_z;

	struct quaternion orientation;

	struct vect accels;

	struct vect world_accels;
};

void update_position(struct position *pos, int left_count, int right_count);

int getAccels(struct position *pos, char *packet);

#endif
