#ifndef ROBOT__H
#define ROBOT__H

#include "motor.h"
#include "quaternion.h"
#include "position.h"
#include "list.h"
#include "../includes/strings.h"
#include "../includes/variable.h"
#include "../includes/fly.h"
#include "../includes/turret.h"

#define STATE_BALANCE 0
#define STATE_CALIBRATING 1

struct robot
{
	int encoder_handle;

	float range;

	struct quaternion orientation;

	float left_count;

	float right_count;

	struct position position;

	char sensor_data[50];

	float tilt_target;

	struct pd* tilt_loop;

	float linear_target;

	struct pd* linear_loop;

	float rotational_target;

	struct pd* spin_loop;
	
	struct motor left_motor;

	struct motor right_motor;

	float linear_velocity;

	float linear_velocity_setpoint;

	float linear_velocity_max;

	float linear_max;

	float linear_acceleration;

	float rotational_velocity;

	float rotational_velocity_max;

	float rotational_velocity_setpoint;

	float rotational_max;

	float rotational_acceleration;

	float in_spin;

	float in_speed;

	float is_following;

  int got_command;

	struct coord *dest;

  struct list_head waypoints;

  float look_angle;

	float spin_seek_const;

	float linear_seek_const;

	struct turret turret;

	int state;

	struct position2 position2;

	int has_traction;

	struct list_head task_list;

	struct server *server;

  struct fly the_fly;

  int set_target;
};

struct robot_task
{
	void (*task_func)(struct robot *);

	struct list_head list_item;
};

void init_robot(struct robot *robot);

void stand_up(struct robot *robot);

void do_robot_tasks(struct robot *robot);

void stay_away(struct robot *robot);

void stay_within(struct robot *robot);

float distance_to(float x1, float y1, float x2, float y2);

float angle_to(struct robot *robot, float x1, float y1, float x2, float y2);

void spin(struct robot *robot);

void move(struct robot *robot);

void adjust_speeds(struct robot *robot);

int num_waypoints(struct robot *robot);

void clear_waypoints(struct robot * robot);

void insert_waypoint_at( struct robot * robot,
			struct coord *point, int index);

void delete_waypoint(struct robot * robot, int index);

void close_loop(struct robot * robot);

void *coord_in(struct variable *var, struct string *string);

void *coord_in_last(struct variable *var, struct string *string);

void *coord_clear(struct variable *var, struct string *string);

void *coord_loop(struct variable *var, struct string *string);

struct string *coord_out(struct variable *var, struct string *string);

float stopping_distance(float speed, float acceleration);
#endif
