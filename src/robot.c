#include "../includes/robot.h"
#include "../includes/pd.h"
#include "../includes/coord.h"
#include "../includes/vector.h"
#include "../includes/fly.h"
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

	robot->tilt_target = -0.05;

	robot->tilt_loop = new_pd(19, 85, 1, 0, 0, &robot->position2.tilt, &robot->tilt_target); 

	robot->linear_loop = new_pd(0.004, 0.0045, 60, 0, 0, &robot->position2.distance_moved, &robot->linear_target); 

	robot->spin_loop = new_pd(-.6, -10, 1, 0, 0, &robot->position2.heading, &robot->rotational_target); 

	gpioInitialise();

	robot->right_motor.pin_a = 9;
	robot->right_motor.pin_b = 11;
	robot->right_motor.enable = 10;

	robot->left_motor.pin_a = 7;
	robot->left_motor.pin_b = 8;
	robot->left_motor.enable = 25;

	init_motor(&robot->left_motor);
	init_motor(&robot->right_motor);

	robot->right_motor.boost = 0;
	robot->left_motor.boost = 0;

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

	robot->linear_seek_const = 0.001;

	robot->spin_seek_const = 0.008;

	robot->turret.roll = 1380;

	robot->turret.pitch = 900;

  init_fly(&robot->the_fly);

	INIT_LIST_HEAD(&robot->task_list);
}

void stand_up(struct robot *robot)
{
	float tilt_output = pd_loop(robot->tilt_loop);

	float linear_output = pd_loop(robot->linear_loop);

	float spin_output = pd_loop(robot->spin_loop);

	if (robot->has_traction)
	{
		set_motor(&robot->left_motor, tilt_output + linear_output - spin_output);
		set_motor(&robot->right_motor, tilt_output + linear_output + spin_output);
	}
	else
	{
		set_motor(&robot->left_motor, 0);
		set_motor(&robot->right_motor, 0);
	}
}

void do_robot_tasks(struct robot *robot)
{
	struct robot_task *temp, *current;

	if (!robot) return;

	list_for_each_entry_safe(current, temp, &robot->task_list, list_item)
	{
		current->task_func(robot);
	}
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

	return atan2f(sinf(angle - robot->position2.heading),
									cosf(angle - robot->position2.heading));
}

void spin(struct robot *robot)
{
  static float angle;

  if (robot->has_traction)
  {
    angle += .001;
  }
  else
  {
    angle = 0;
  }
}

void stay_away(struct robot *robot)
{
  float x_diff = robot->turret.target.x - robot->position2.position.x;
  float y_diff = robot->turret.target.y - robot->position2.position.y;

  float new_x, new_y;
  
  float distance = sqrtf(x_diff * x_diff + y_diff * y_diff);

  if (distance < 700)
  {
    float angle = normalized_angle(robot->look_angle);

    new_x = robot->turret.target.x - cosf(angle) * 600;
    new_y = robot->turret.target.y - sinf(angle) * 600;

    x_diff = new_x - robot->position2.position.x;
    y_diff = new_y - robot->position2.position.y;

    distance = sqrtf(x_diff * x_diff + y_diff * y_diff);

    if (distance > 20)
    {
      robot->dest->x = new_x;
      robot->dest->y = new_y;
    }
  }

  //printf("%f, %f, %f, %f, %f, %f, %f\n", robot->position2.position.x, robot->position2.position.y, robot->turret.target.x, robot->turret.target.y, distance, robot->dest->x, robot->dest->y);
}

void stay_within(struct robot *robot)
{
  float distance = sqrtf(robot->dest->x * robot->dest->x + 
    robot->dest->y * robot->dest->y);

  if (distance > 500)
  {
    robot->dest->x = robot->dest->x / distance;
    robot->dest->y = robot->dest->y / distance;
  }
}

void move(struct robot *robot)
{
  float spin_angle = normalized_angle(robot->look_angle -
    robot->position2.heading);

  static float x_diff, y_diff;

  if (robot->dest)
  {
    x_diff = robot->dest->x - robot->position2.position.x;
    y_diff = robot->dest->y - robot->position2.position.y;

    float distance = sqrtf((x_diff * x_diff) + (y_diff * y_diff));

    if (distance > 100)
    {
      static float angle;

      angle = atan2f(y_diff, x_diff) - robot->position2.heading;

      angle = normalized_angle(angle);

      float face_towards = normalized_angle(robot->look_angle - atan2f(y_diff, x_diff));

      if (fabs(face_towards) < M_PI / 2.0)
      {
        robot->rotational_velocity_setpoint = robot->spin_seek_const * angle;
      }
      else
      {
        robot->rotational_velocity_setpoint = -robot->spin_seek_const * angle;
      }

      if (stopping_distance(robot->rotational_velocity,
        robot->rotational_acceleration) >= fabs(angle))
      {
        robot->rotational_velocity_setpoint = 0;
      }

      robot->linear_velocity_setpoint = robot->linear_seek_const * distance
        * cosf(angle);

      if (stopping_distance(robot->linear_velocity,
        robot->linear_acceleration) >= distance)
      {
        robot->linear_velocity_setpoint = 0;
      }

    }
    else
    {
      robot->rotational_velocity_setpoint = robot->spin_seek_const * spin_angle;

      if (stopping_distance(robot->rotational_velocity,
        robot->rotational_acceleration) >= fabs(spin_angle))
      {
        robot->rotational_velocity_setpoint = 0;
      }

      robot->linear_velocity_setpoint = 0;
    }
  }
}

void adjust_speeds(struct robot *robot)
{
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
		sprintf(buffer, "none,none");
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

