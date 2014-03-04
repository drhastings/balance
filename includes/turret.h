#ifndef _TURRET_H
#define _TURRET_H

#include "quaternion.h"

struct turret
{
	float pan;

	float tilt;

  float pitch_guess;

  float yaw_guess;

	float roll;

	float yaw;

	float pitch;

	struct vect target;

	struct quaternion turret_rotation;
};

float get_expected_distance(struct robot *robot);

void follow(struct robot *robot);

void pointer(struct robot *robot);

int point_at(struct robot *robot, struct vect *point);

void pan_tilt_to_pitch_roll(struct turret *turret);

void screen_to_world_vect(struct robot *robot, struct vect *out,
                            float x, float y);

void cam_to_local(struct robot *robot, struct vect *out, struct vect *in);

void cam_to_world(struct robot *robot, struct vect *out, struct vect *in);

#endif
