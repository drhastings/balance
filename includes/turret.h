#ifndef _TURRET_H
#define _TURRET_H

#include "quaternion.h"

struct turret
{
	float pan;

	float tilt;

	float roll;

	float yaw;

	float pitch;

	struct vect target;

	struct quaternion turret_rotation;
};

void follow(struct robot *robot);

void pointer(struct robot *robot);

int point_at(struct robot *robot, struct vect *point);

void pan_tilt_to_pitch_roll(struct turret *turret);

#endif
