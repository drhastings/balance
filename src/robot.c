#include "../includes/robot.h"
#include "../includes/pd.h"
#include "../includes/coord.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <string.h>
#include <pigpio.h>

#define  ENCODER_FILE "/dev/encoders"

float stopping_distance(float speed, float acceleration);

void init_robot(struct robot *robot)
{
	memset(robot, 0, sizeof(*robot));

	robot->encoder_handle = open(ENCODER_FILE, O_RDONLY);

	robot->tilt_target = -0.15;

	robot->tilt_loop = new_pd(14, 55, 1, 0, 0, &robot->position.tilt, &robot->tilt_target); 

	robot->linear_loop = new_pd(0.003, 0.0033, 60, 0, 0, &robot->position.linear_count, &robot->linear_target); 

	robot->spin_loop = new_pd(.8, 8, 1, 0, 0, &robot->position.incr_inertial_heading, &robot->rotational_target); 

	gpioInitialise();

	robot->left_motor.pin_a = 11;
	robot->left_motor.pin_b = 9;
	robot->left_motor.enable = 10;

	robot->right_motor.pin_a = 7;
	robot->right_motor.pin_b = 8;
	robot->right_motor.enable = 25;

	init_motor(&robot->left_motor);
	init_motor(&robot->right_motor);

	robot->right_motor.boost = 60;
	robot->left_motor.boost = 60;

	robot->left_motor.min = 20;
	robot->right_motor.min = 20;

	robot->linear_velocity = 0;

	robot->linear_velocity_setpoint = 0;

	robot->linear_velocity_max = 8;

	robot->linear_acceleration = 0.02;

	robot->rotational_velocity = 0;

	robot->rotational_velocity_setpoint = 0;

	robot->rotational_velocity_max = 0.02;

	robot->rotational_acceleration = 0.0004;

	robot->position.wheel_base = 90;

	robot->linear_seek_const = 0.04;

	robot->spin_seek_const = 0.012;

	robot->turret.roll = 1380;

	robot->turret.pitch = 900;
}

float distance_to(float x1, float y1, float x2, float y2)
{
	float x_diff = x2 - x1;

	float y_diff = y2 - y1;

	return sqrtf(x_diff * x_diff + y_diff * y_diff);
}

float angle_to(struct robot *robot, float x1, float y1, float x2, float y2)
{
	float x_diff = x2 - x1;

	float y_diff = y2 - y1;

	float angle = atan2f(y_diff, x_diff);

	return atan2f(sinf(angle - robot->position.incr_inertial_heading),
									cosf(angle - robot->position.incr_inertial_heading));
}

void move(struct robot *robot)
{
	if (robot->dest && robot->is_following)
	{
		float dist = distance_to(robot->position.x_inertial,
			robot->position.y_inertial, robot->dest->x, robot->dest->y);

		//printf("%f\n", dist);

		float clipped_angle = angle_to(robot, robot->position.x_inertial,
					robot->position.y_inertial, robot->dest->x, robot->dest->y);

		if(dist > 20)
		{
			robot->linear_velocity_setpoint = robot->linear_seek_const * dist *
				cosf(clipped_angle);

			if (robot->linear_velocity_setpoint < 0) robot->linear_velocity_setpoint = 0;

			robot->rotational_velocity_setpoint = robot->spin_seek_const * clipped_angle;

			if (stopping_distance(robot->linear_velocity,
						robot->linear_acceleration) > dist)
			{
				robot->linear_velocity_setpoint = 0;
			}

			if (stopping_distance(robot->rotational_velocity,
						robot->rotational_acceleration) > fabs(clipped_angle))
			{
				robot->rotational_velocity_setpoint = 0;
			}
		}
		else
		{
			robot->dest = robot->dest->next;

			if (!robot->dest) robot->is_following = 0;
		}
	}
	else
	{
		robot->rotational_velocity_setpoint = -robot->in_spin * robot->rotational_velocity_max;

		robot->linear_velocity_setpoint = robot->in_speed * robot->linear_velocity_max;
	}

	//printf("%f\n", robot->range);
	//printf("%f\n", robot->turret.tilt);
	if ((robot->range < stopping_distance(robot->linear_velocity,
					robot->linear_acceleration) + 450 || robot->range <= 300)
						 && robot->turret.tilt > -33)
	{
		//printf("Too close!\n");
		if (robot->linear_velocity_setpoint > 0)
			robot->linear_velocity_setpoint = 0;
	}
	else
	{
		robot->linear_velocity_setpoint = 2;
	}

	if (robot->range < 500)
	{
		robot->rotational_velocity_setpoint = .003;
	}
	else
	{
		robot->rotational_velocity_setpoint = .0;
	}

	if (robot->rotational_velocity_setpoint > robot->rotational_velocity_max)
		robot->rotational_velocity_setpoint = robot->rotational_velocity_max;

	if (robot->rotational_velocity_setpoint < -robot->rotational_velocity_max)
		robot->rotational_velocity_setpoint = -robot->rotational_velocity_max;

	if (robot->linear_velocity_setpoint > robot->linear_velocity_max)
		robot->linear_velocity_setpoint = robot->linear_velocity_max;

	if (robot->linear_velocity_setpoint < -robot->linear_velocity_max)
		robot->linear_velocity_setpoint = -robot->linear_velocity_max;

	if (robot->linear_velocity > robot->linear_velocity_setpoint)
	{
		if (robot->linear_velocity - robot->linear_velocity_setpoint <
					robot->linear_acceleration)
			robot->linear_velocity = robot->linear_velocity_setpoint;
		else
			robot->linear_velocity -= robot->linear_acceleration;
	}
	if (robot->linear_velocity < robot->linear_velocity_setpoint)
	{
		if (robot->linear_velocity_setpoint - robot->linear_velocity <
					robot->linear_acceleration)
			robot->linear_velocity = robot->linear_velocity_setpoint;
		else
			robot->linear_velocity += robot->linear_acceleration;
	}
	if (robot->rotational_velocity > robot->rotational_velocity_setpoint)
	{
		if (robot->rotational_velocity - robot->rotational_velocity_setpoint <
					robot->rotational_acceleration)
			robot->rotational_velocity = robot->rotational_velocity_setpoint;
		else
			robot->rotational_velocity -= robot->rotational_acceleration;
	}
	if (robot->rotational_velocity < robot->rotational_velocity_setpoint)
	{
		if (robot->rotational_velocity_setpoint - robot->rotational_velocity <
					robot->rotational_acceleration)
			robot->rotational_velocity = robot->rotational_velocity_setpoint;
		else
			robot->rotational_velocity += robot->rotational_acceleration;
	}

	if (robot->linear_velocity > robot->linear_velocity_max)
		robot->linear_velocity = robot->linear_velocity_max;

	if (robot->linear_velocity < -robot->linear_velocity_max)
		robot->linear_velocity = -robot->linear_velocity_max;

	if (robot->linear_velocity > robot->linear_velocity_max)
		robot->rotational_velocity = robot->rotational_velocity_max;

	if (robot->rotational_velocity < -robot->rotational_velocity_max)
		robot->rotational_velocity = -robot->rotational_velocity_max;

	robot->linear_target += robot->linear_velocity;
	robot->rotational_target += robot->rotational_velocity;
}

void *coord_in(struct variable *var, struct string *string)
{
	struct robot *robot = (struct robot *)var->value;

	char *index = string->buffer;

	int number;

	float x, y;

	number = atoi(string->buffer);

	while (index - string->buffer < string->length)
	{
		if (*index != ':')
		{
			index++;
		}
		else
		{
			index++;

			break;
		}
	}

	x = atof(index);

	while (index - string->buffer < string->length)
	{
		if (*index != ':')
		{
			index++;
		}
		else
		{
			index++;

			break;
		}
	}

	y = atof(index);

	insert_waypoint_at(robot, new_coord(x, y), number);

  return 0;
}

void *coord_in_last(struct variable *var, struct string *string)
{
	struct robot *robot = (struct robot *)var->value;

	char *index = string->buffer;

	float x, y;

	x = atof(string->buffer);

	while (index - string->buffer < string->length)
	{
		if (*index != ':')
		{
			index++;
		}
		else
		{
			index++;

			break;
		}
	}

	y = atof(index);

	insert_waypoint_at(robot, new_coord(x, y), num_waypoints(robot));

  return 0;
}

void *coord_clear(struct variable *var, struct string *string)
{
	struct robot *robot = (struct robot *)var->value;

	clear_waypoints(robot);

  return 0;
}

void *coord_loop(struct variable *var, struct string *string)
{
	struct robot *robot = (struct robot *)var->value;

	close_loop(robot);

	return 0;
}

int num_waypoints(struct robot *robot)
{
	struct coord *dest = robot->dest;
	struct coord *head;

	int ret = 0;

	if (!dest)
		return ret;

	head = dest;

	ret++;

	while (dest->next)
	{
		if (dest->next == head)
			break;

		dest = dest->next;

		ret++;
	}

	return ret;
}

void clear_waypoints(struct robot * robot)
{
	int to_clear = num_waypoints(robot);

	struct coord *next;
	struct coord *this = robot->dest;

	int x;

	for (x = 0; x < to_clear; x++)
	{
		next = this->next;

		free(this);

		this = next;
	}

	robot->dest = NULL;
}

void insert_waypoint_at( struct robot * robot,
			struct coord *point, int index)
{
	struct coord *this = robot->dest;

	if (!index)
	{
		if (robot->dest)
			point->next = robot->dest->next;

		robot->dest = point;

		return;
	}

	struct coord *before = this;

	index--;

	while (index)
	{
		if (before->next)
			before = before->next;

		index--;
	}

	point->next = before->next;

	before->next = point;
}

void delete_waypoint(struct robot * robot, int index);

void close_loop(struct robot * robot)
{
	struct coord *last = robot->dest;

	int waypoints = num_waypoints(robot);

	waypoints--;

	while (waypoints)
	{
		last = last->next;

		waypoints--;
	}

	if (last)
		last->next = robot->dest;
}

struct string *coord_out(struct variable *var, struct string *string)
{
	char buffer[64];

	struct robot *robot = (struct robot*)var->value;

	memset(buffer, 0, 64);

	if (!robot->dest)
	{
		sprintf(buffer, "none");
	}
	else
	{
		sprintf(buffer, "%f,%f", robot->dest->x, robot->dest->y);
	}

	return new_string(buffer);
}

float stopping_distance(float speed, float acceleration)
{
	return (speed * speed) / (2 * acceleration);
}

