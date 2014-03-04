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
	.y = -240,
	.z = -70
};


void world_to_local(struct robot * robot, struct vect *local,
	struct vect *world)
{
	struct quaternion backwards;

  struct vect temp = *world;

	temp.x -= robot->position2.position.x;
	temp.y -= robot->position2.position.y;

	get_conjugate(&backwards, &robot->position2.corrected_orientation);

	rotate_vector(local, &temp, &backwards);
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
	//printf("%f, %f, %f\n", point->x, point->y, point->z);

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

	turret->yaw = (atan2f(normalized_local.y, -normalized_local.x));

	float distance = sqrtf(normalized_local.x * normalized_local.x +
				normalized_local.y * normalized_local.y);

	turret->pitch = -(atan2f(normalized_local.z, distance));

//  fprintf(stderr, "%f\n", turret->pitch);

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

	float yaw_setting = turret->yaw * conversion_factor + 415;

	float pitch_setting = 2125 - (turret->pitch * conversion_factor);

  if (yaw_setting > 500 && yaw_setting < 2500)
  {
  	gpioServo(24, yaw_setting);
  }

  if (pitch_setting > 500 && pitch_setting < 2500)
  {
  	gpioServo(4, pitch_setting);
  }

	if (can_point)
	{
		//gpioWrite(14, 1);
	}
	else
	{
		//gpioWrite(14, 0);
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

float get_expected_distance(struct robot *robot)
{
  struct vect local_look, look, offset, world_offset, offset_cam, cam_in_world, floor_spot;

  look.x = 0;
  look.y = 10;
  look.z = 0;

  offset.x = 0;
  offset.y = 0;
  offset.z = 150;

  cam_to_world(robot, &local_look, &look);

  cam_to_world(robot, &world_offset, &offset);

  local_to_world(robot, &cam_in_world, &camera_center);

  offset_cam.x = cam_in_world.x + world_offset.x;
  offset_cam.y = cam_in_world.y + world_offset.y;
  offset_cam.z = cam_in_world.z + world_offset.z;

  find_floor_intersection(&floor_spot, &offset_cam, &local_look);

  float xx = (floor_spot.x - cam_in_world.x) * (floor_spot.x - cam_in_world.x);
  float yy = (floor_spot.y - cam_in_world.y) * (floor_spot.y - cam_in_world.y);
  float zz = (floor_spot.z - cam_in_world.z) * (floor_spot.z - cam_in_world.z);

  return sqrtf(xx + yy +zz) - 35;
}

void screen_to_world_vect(struct robot *robot, struct vect *out,
                            float x, float y)
{
  struct vect local_look, look, cam_in_world, floor_spot;

  look.x = 200 * x; 
  look.y = 1000; 
  look.z = 200 * y; 

  cam_to_world(robot, &local_look, &look);

  local_to_world(robot, &cam_in_world, &camera_center);

//  fprintf(stderr, "%f, %f, %f\n", local_look.x, local_look.y, local_look.z);

  find_floor_intersection(&floor_spot, &cam_in_world, &local_look);

  *out = floor_spot;

//  fprintf(stderr, "%f, %f, %f\n", out->x, out->y, out->z);
}

void cam_to_local(struct robot *robot, struct vect *out, struct vect *in)
{
  struct vect temp_look, look;

  temp_look = *in;

  look.z = cosf(-robot->turret.roll) * temp_look.z - 
                  sinf(-robot->turret.roll) * temp_look.x;
  look.x = sinf(-robot->turret.roll) * temp_look.z + 
                  cosf(-robot->turret.roll) * temp_look.x;

  look.y = temp_look.y;

  temp_look = look;

  look.y = cosf(-robot->turret.pitch) * temp_look.y - 
                  sinf(-robot->turret.pitch) * temp_look.z;
  look.z = sinf(-robot->turret.pitch) * temp_look.y + 
                  cosf(-robot->turret.pitch) * temp_look.z;

  temp_look = look;

  look.y = cosf(robot->turret.yaw - M_PI / 2) * temp_look.y - 
                  sinf(robot->turret.yaw - M_PI / 2) * temp_look.x;
  look.x = sinf(robot->turret.yaw - M_PI / 2) * temp_look.y + 
                  cosf(robot->turret.yaw - M_PI / 2) * temp_look.x;

//  fprintf(stderr, "%f, %f, %f\n", look.x, look.y, look.z);
  *out = look;
}

void cam_to_world(struct robot *robot, struct vect *out, struct vect *in)
{
  struct vect temp;

  cam_to_local(robot, &temp, in);

  local_to_world(robot, out, &temp);

  out->x -= robot->position2.position.x;
  out->y -= robot->position2.position.y;
}

void follow(struct robot *robot)
{
	float conversion_factor = (1000.0 / 90.0) * (180 / M_PI);

 	if (robot->turret.yaw * conversion_factor > 1960)
	{
		robot->turret.yaw = M_PI / 2;
		robot->turret.pitch = M_PI / 2;
	}
	if (robot->turret.yaw * conversion_factor < 160)
	{
		robot->turret.yaw = M_PI / 2;
		robot->turret.pitch = M_PI / 4;
	}
	if (robot->turret.pitch * conversion_factor > 1440)
	{
		robot->turret.yaw = M_PI / 2;
		robot->turret.pitch = M_PI / 4;
	}
	if (robot->turret.pitch *conversion_factor < 140)
	{
		robot->turret.yaw = M_PI / 2;
		robot->turret.pitch = M_PI / 4;
	}

	float yaw_setting = robot->turret.yaw * conversion_factor + 340;

	float pitch_setting = robot->turret.pitch * conversion_factor + 560;

	gpioServo(24, yaw_setting);
	gpioServo(4, pitch_setting); 
}

void pointer(struct robot *robot)
{
  struct vect point;

  point.x = (cosf(robot->rotational_target + (robot->rotational_velocity * 0)) * 1000) + robot->position2.position.x;
  point.y = (sinf(robot->rotational_target + (robot->rotational_velocity * 0)) * 1000) + robot->position2.position.y;
  point.z = -40;

  //fprintf(stderr, "%f, %f, %f\n\n", robot->turret.target.x, robot->turret.target.y, robot->turret.target.z);

	point_at_world(robot, &robot->turret.target);

  struct vect cam_in_world;

  local_to_world(robot, &cam_in_world, &camera_center);

//  fprintf(stderr, "%f, %f, %f\n", cam_in_world.x, cam_in_world.y, cam_in_world.z);
//  fprintf(stderr, "%f, %f\n", robot->position2.position.x, robot->position2.position.y);

  struct vect d;

  difference(&d, &cam_in_world, &point);

  float xx = d.x * d.x;

  float yy = d.y * d.y;

  float zz = d.z * d.z;

  float predicted_distance = sqrtf(xx + yy + zz) - 40;

  //fprintf(stderr, "%f, %f\n", predicted_distance, robot->range);

  if (predicted_distance < robot->range)
  {
    gpioWrite(14, 0);
  }
   else
  {
    gpioWrite(14, 0);
  }

	struct vect output;

	get_up_vector(robot, &output);

//	printf("%f, %f, %f\n", output.x, output.y, output.z);

	struct vect cam_up, up, look, cam_forward, product;

	up.x = 0;
	up.y = 0;
	up.z = 1.0;

	cam_forward.x = 0;
	cam_forward.y = -1;
	cam_forward.z = 0;

	rotate_vector(&cam_up, &up, &robot->turret.turret_rotation);
	rotate_vector(&look, &cam_forward, &robot->turret.turret_rotation);

	robot->turret.roll = acosf(dot(&cam_up, &output));

	cross(&cam_up, &output, &product);

	robot->turret.roll = -asinf(product.x / look.x);

  if (robot->turret.pitch > robot->turret.pitch_guess)
  {
    robot->turret.pitch_guess += 0.000000174532925;
 
    if (robot->turret.pitch_guess > robot->turret.pitch)
    {
      robot->turret.pitch_guess = robot->turret.pitch;
    }
  }

  if (robot->turret.pitch < robot->turret.pitch_guess)
  {
    robot->turret.pitch_guess -= 0.000000174532925;
 
    if (robot->turret.pitch_guess < robot->turret.pitch)
    {
      robot->turret.pitch_guess = robot->turret.pitch;
    }
  } 

  if (robot->turret.yaw > robot->turret.yaw_guess)
  {
    robot->turret.yaw_guess += 0.000000174532925;
 
    if (robot->turret.yaw_guess > robot->turret.yaw)
    {
      robot->turret.yaw_guess = robot->turret.yaw;
    }
  }

  if (robot->turret.yaw < robot->turret.yaw_guess)
  {
    robot->turret.yaw_guess -= 0.000000174532925;
 
    if (robot->turret.yaw_guess < robot->turret.yaw)
    {
      robot->turret.yaw_guess = robot->turret.yaw;
    }
  } 

	//printf("%f, %f, %f\n", local.x, local.y, local.z);
	//fprintf(stderr, "%f\n", robot->turret.roll);
}

