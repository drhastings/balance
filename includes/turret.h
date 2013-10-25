#ifndef _TURRET_H
#define _TURRET_H

#include "quaternion.h"

struct turret
{
	float pan;

	float tilt;

	float roll;

	float pitch;

	struct quaternion turret_rotation;
};

void pan_tilt_to_pitch_roll(struct turret *turret);

#endif
