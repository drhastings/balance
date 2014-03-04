#ifndef POSITION__H
#define POSITION__H

#include <math.h>
#include "quaternion.h"
#include "vector.h"

#define G 9806.65f

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

	float traction_value;
};

struct position2
{
	struct quaternion orientation;
	struct quaternion corrected_orientation;

	struct vect accels;
	struct vect local_accels;
	struct vect world_accels;

	struct vect imu_velocity;

	struct vect imu_position;

	struct vect wheel_position;

	struct vect local_wheel_coord_imu;
	struct vect world_wheel_coord_imu;
	struct vect last_world_wheel_coord_imu;

	struct vect local_wheel_coord_enc;
	struct vect world_wheel_coord_enc;
	struct vect last_world_wheel_coord_enc;

	int left;
	int right;

	float last_left;
	float last_right;

	float movement;
	float distance_moved;

	float heading;
	float wheel_heading_change;
	float last_imu_heading;
	float heading_offset;

	float tilt;
  float tilt_target;

	struct vect position;

	struct vect forward;
	struct vect gravity;

	float wheel_base;
};

void init_position(struct position2 *pos);

void update_position(struct position *pos, int left_count, int right_count);

void update_position2(struct robot *robot);

int get_sensor_data(struct position2 *pos, char *packet);

void update_wheel_position(struct position2 *pos);

void update_imu_position(struct position2 *pos);

void update_tilt(struct position2 *pos);

void update_wheel_coord_imu(struct position2 *pos);

float imu_heading(struct position2 *pos);

int getAccels(struct position *pos, char *packet);

#endif
