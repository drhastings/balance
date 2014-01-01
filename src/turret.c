#include "../includes/turret.h"
#include "../includes/state.h"
#include "../includes/vector.h"
#include "../includes/coord.h"
#include "../includes/robot.h"
#include "../includes/quaternion.h"
#include <math.h>
#include <stdio.h>
#include <pigpio.h>

struct vect camera_center =
{
	.x = 0,
	.y = 240,
	.z = 90
};


void world_to_local(struct robot * robot, struct vect *local,
	struct vect *world)
{
	struct quaternion backwards;

	world->x -= robot->position2.position.x;
	world->y -= robot->position2.position.y;

	get_conjugate(&backwards, &robot->position2.corrected_orientation);

	rotate_vector(local, world, &backwards);
}

void local_to_world(struct robot * robot, struct vect *world,
	struct vect *local)
{
	rotate_vector(world, local, &robot->position2.corrected_orientation);

	struct vect out;

	rotate_vector(&out, &camera_center, &robot->position2.corrected_orientation);

	//printf("%f, %f, %f\n", out.x, out.y, out.z);

	world->x += robot->position2.position.x;
	world->y += robot->position2.position.y;
}

void get_up_vector(struct robot *robot, struct vect *result)
{
	struct vect up, local_up, forward, look;

	up.x = robot->position2.position.x;
	up.y = robot->position2.position.y;
	up.z = 1;

	world_to_local(robot, &local_up, &up);

	forward.x = 0;
	forward.y = -1;
	forward.z = 0;

	struct vect normalized_look;

	rotate_vector(&look, &forward, &robot->turret.turret_rotation);

//	printf("%f, %f, %f\n", look.x, look.y, look.z);

	float normal;

	struct vect right_vector, raw;

	cross(&local_up, &look, &right_vector);

//	printf("%f, %f, %f\n", right_vector.x, right_vector.y, right_vector.z);
//	printf("%f\n", norm(&right_vector));

	cross(&right_vector, &look, &raw);

//	printf("%f, %f, %f\n", raw.x, raw.y, raw.z);

	normal = norm(&raw);

	result->x = raw.x / normal;
	result->y = raw.y / normal;
	result->z = raw.z / normal;
}
	

void rotate_on_camera(struct turret *turret, struct vect *result,
		struct vect* input)
{
	struct vect diff;

	rotate_vector(&diff, input, &turret->turret_rotation);

	//printf("%f, %f, %f\n", diff.x, diff.y, diff.z);

	result->x = diff.x + camera_center.x;
	result->y = diff.y + camera_center.y;
	result->z = diff.z + camera_center.z;
}

int point_at_local(struct turret *turret, struct vect *point)
{
	int can_point = 1;

//	point->x = 0;
//	point->y = -40;
//	point->z = 400;

	struct vect normalized_local;

	struct vect diff;

	difference(&diff, point, &camera_center);

	//printf("%f, %f, %f\n", diff.x, diff.y, diff.z);

	float magnitude = norm(&diff);

	if (magnitude == 0)
	{
		normalized_local.x = 0;
		normalized_local.y = 0;
		normalized_local.z = 1.0;
	}
	else
	{
		normalized_local.x = diff.x / magnitude;
		normalized_local.y = diff.y / magnitude;
		normalized_local.z = diff.z / magnitude;
	}

	float conversion_factor = (1000.0 / 90.0) * (180 / M_PI);

	turret->yaw = (atan2f(-normalized_local.y, normalized_local.x));

	float distance = sqrtf(normalized_local.x * normalized_local.x +
				normalized_local.y * normalized_local.y);

	turret->pitch = (atan2f(normalized_local.z, distance));

	if (turret->yaw * conversion_factor > 1960)
	{
		can_point =0;
		turret->yaw = M_PI / 2;
		turret->pitch = M_PI / 4;
	}
	if (turret->yaw * conversion_factor < 160)
	{
		can_point = 0;
		turret->yaw = M_PI / 2;
		turret->pitch = M_PI / 4;
	}
	if (turret->pitch * conversion_factor > 1440)
	{
		can_point = 0;
		turret->yaw = M_PI / 2;
		turret->pitch = M_PI / 4;
	}
	if (turret->pitch *conversion_factor < 140)
	{
		can_point = 0;
		turret->yaw = M_PI / 2;
		turret->pitch = M_PI / 4;
	}

	float yaw_setting = turret->yaw * conversion_factor + 340;

	float pitch_setting = turret->pitch * conversion_factor + 560;

	gpioServo(24, yaw_setting);
	gpioServo(4, pitch_setting);

	if (can_point)
	{
		gpioWrite(14, 1);
	}
	else
	{
		gpioWrite(14, 0);
	}

	struct quaternion yaw, pitch;

	yaw.w = cosf((turret->yaw - M_PI / 2) / 2);
	yaw.x = 0;
	yaw.y = 0;
	yaw.z = sinf((turret->yaw - M_PI / 2) / 2);

	pitch.w = cosf(turret->pitch / 2);
	pitch.x = -sinf(turret->pitch / 2);
	pitch.y = 0;
	pitch.z = 0;

	quaternion_product(&turret->turret_rotation, &yaw, &pitch);

	return can_point;
}

int point_at_world(struct robot *robot, struct vect *point)
{
	struct vect local_point;

	world_to_local(robot, &local_point, point);

	return point_at_local(&robot->turret, &local_point);
}

void pointer(struct robot *robot)
{
	struct vect target;

	static float angle = 0;

	target.x = 500;
	target.y = 0;
	target.z = -40;

//	angle += .01;

	if (angle > 2 * M_PI) angle = 0;

	point_at_world(robot, &target);

	struct vect local_point, local, output, diff;

	local_point.x = 0;
	local_point.y = -500;
	local_point.z = 0;

	rotate_on_camera(&robot->turret, &local, &local_point);

	local_to_world(robot, &output, &local);

	get_up_vector(robot, &output);

//	printf("%f, %f, %f\n", output.x, output.y, output.z);

	robot->turret.roll = atan2f(output.x, output.y);

	struct vect cam_up, up;

	up.x = 0;
	up.y = 0;
	up.z = 1.0;

	rotate_vector(&cam_up, &up, &robot->turret.turret_rotation);

	robot->turret.roll = acosf(dot(&cam_up, &output));

	//printf("%f, %f, %f\n", local.x, local.y, local.z);
	fprintf(stderr, "%f\n", robot->turret.roll);
}

