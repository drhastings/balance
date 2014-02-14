#include "../includes/position.h"
#include "../includes/quaternion.h"
#include "../includes/robot.h"
#include "../includes/vector.h"
#include <stdio.h>

#include "../includes/quaternion.h"

void init_position(struct position2 *pos)
{
	pos->local_wheel_coord_imu.x = 0;
	pos->local_wheel_coord_imu.y = -230;
	pos->local_wheel_coord_imu.z = -75;

	pos->wheel_base = 90;

	pos->last_left = pos->left;
	pos->last_right = pos->right;

	//printf("%f, %f\n", pos->last_left, pos->last_right);

	pos->position.x = 0;
	pos->position.y = 0;
	pos->position.z = 0;

	pos->heading = imu_heading(pos);
}

void update_position(struct position *pos, int left_count, int right_count)
{
	float tilt_encoder_offset = -sinf(pos->tilt) * (360 / M_PI);

	float left_movement = left_count - pos->last_left + tilt_encoder_offset;
	pos->last_left = left_count + tilt_encoder_offset;

	float right_movement = right_count - pos->last_right + tilt_encoder_offset;
	pos->last_right = right_count + tilt_encoder_offset;

	float movement = ((left_movement + right_movement) / 2.0);

	pos->linear_count += movement; 

	movement = movement * 0.349065850398865;

	float incremental_odometry_angle = ((right_movement - left_movement) *
			 0.349065850398865) / pos->wheel_base;

	pos->heading += incremental_odometry_angle;

	float incremental_angle = pos->inertial_heading - 
			pos->last_inertial_heading;

	while (incremental_angle > 1.5)
	{
		incremental_angle -= M_PI;
	}

	while (incremental_angle < -1.5)
	{
		incremental_angle += M_PI;
	}

	pos->incr_inertial_heading += incremental_angle;

	pos->last_inertial_heading = pos->inertial_heading;

	pos->traction_value = incremental_angle - incremental_odometry_angle;

	if (fabs(incremental_angle - incremental_odometry_angle) < .015 &&
				fabs(pos->tilt) < 0.55)
	{
		pos->has_traction = 1;
	}
	else
	{
		pos->has_traction = 0;
	}

	pos->x += movement * cosf(pos->heading);

	pos->y += movement * sinf(pos->heading);

	pos->x_inertial += movement * cosf(pos->incr_inertial_heading);

	pos->y_inertial += movement * sinf(pos->incr_inertial_heading);
}

int get_sensor_data(struct position2 *pos, char *packet)
{
	int16_t temp[4];

	temp[0] = ((packet[0] << 8) + packet[1]);
	temp[1] = ((packet[4] << 8) + packet[5]);
	temp[2] = ((packet[8] << 8) + packet[9]);
	temp[3] = ((packet[12] << 8) + packet[13]);

	pos->orientation.w = (float)temp[0] / 16384.0f;
	pos->orientation.x = (float)temp[1] / 16384.0f;
	pos->orientation.y = (float)temp[2] / 16384.0f;
	pos->orientation.z = (float)temp[3] / 16384.0f;

	temp[0] = ((packet[28] << 8) + packet[29]);
	temp[1] = ((packet[32] << 8) + packet[33]);
	temp[2] = ((packet[36] << 8) + packet[37]);

	pos->accels.x = temp[0];
	pos->accels.y = temp[1];
	pos->accels.z = temp[2];

	pos->left = *((int *) (packet + 42));
	pos->right = *((int *) (packet + 46));

	//printf("%i, %i\n", pos->left, pos->right);

	return 0;
}

int getAccels(struct position *pos, char *packet)
{
	int16_t temp[3];

	temp[0] = ((packet[28] << 8) + packet[29]);
	temp[1] = ((packet[32] << 8) + packet[33]);
	temp[2] = ((packet[36] << 8) + packet[37]);

	pos->accels.x = temp[0];
	pos->accels.y = temp[1];
	pos->accels.z = temp[2];

	pos->world_accels.x = pos->accels.x - pos->orientation.gravity.x * 8192;

	pos->world_accels.y = pos->accels.y - pos->orientation.gravity.y * 8192;

	pos->world_accels.z = pos->accels.z - pos->orientation.gravity.z * 8192;

	//printf("%f, %f, %f\n", pos->accels.x, pos->accels.y, pos->accels.z);
	//printf("%f, %f, %f\n", pos->world_accels.x, pos->world_accels.y, pos->world_accels.z);

	return 0;
}

void update_position2(struct robot *robot)
{
	update_imu_position(&robot->position2);

	struct vect forward, down, projection;

	forward.x = 0;
	forward.y = 0;
	forward.z = 1;

	down.x = 0;
	down.y = -1;
	down.z = 0;

	rotate_vector(&robot->position2.forward, &forward, &robot->position2.orientation);
	rotate_vector(&robot->position2.gravity, &down, &robot->position2.orientation);

	projection = robot->position2.forward;

	projection.z = 0;

	float normal = norm(&projection);

	projection.x = projection.x / normal;
	projection.y = projection.y / normal;

//	printf("%f	%f	%f\n", robot->position2.forward.x, robot->position2.forward.y, robot->position2.forward.z);
//	printf("%f	%f	%f\n", projection.x, projection.y, projection.z);
//	printf("%f	%f	%f\n", robot->position2.gravity.x, robot->position2.gravity.y, robot->position2.gravity.z);

	float heading = atan2f(projection.y, projection.x);

//	printf("%f\n", heading);

	robot->position2.tilt = acosf(dot(&projection, &robot->position2.gravity)) -
    M_PI / 2;

	//printf("%f\n", robot->position2.tilt);

//	update_wheel_coord_imu(&robot->position2);

	update_wheel_position(&robot->position2);

//	printf("%f\n", heading);
//	printf("%i, %i\n", robot->position2.left, robot->position2.right);

	float change = normalized_angle(heading - robot->position2.last_imu_heading);

	if (change > M_PI / 8)
	{
		change = 0;
		robot->position2.heading_offset = heading;
	}

//	printf("%f\n", robot->position2.heading);

	robot->position2.last_imu_heading = heading;

	robot->position2.heading += change;

	robot->position2.position.x += robot->position2.movement *
				cosf(robot->position2.heading);

	robot->position2.position.y += robot->position2.movement *
				sinf(robot->position2.heading);

//	printf("%f, %f\n", robot->position2.position.x, robot->position2.position.y);

	if (fabs(robot->position2.tilt - robot->tilt_target) < .35 && 
       robot->position2.gravity.z > .5)
	{
		robot->has_traction = 1;
	}
	else
	{
		robot->position2.heading_offset = robot->position2.last_imu_heading;

		robot->position2.position.x = 0;
		robot->position2.position.y = 0;
		robot->position2.heading = 0;
		robot->has_traction = 0;
		robot->linear_target = robot->position2.distance_moved;
		robot->rotational_target = 0;
	}

	struct quaternion correction;

	correction.w = cosf(robot->position2.heading_offset / 2);
	correction.x = 0;
	correction.y = 0;
	correction.z = -sinf(robot->position2.heading_offset / 2);

	quaternion_product(&robot->position2.corrected_orientation, &correction,
		&robot->position2.orientation);
}

void update_tilt(struct position2 *pos)
{
	float xx = pos->gravity.x * pos->gravity.x;
	float yy = pos->gravity.y * pos->gravity.y;
	
	pos->tilt = atan2f(pos->gravity.y, pos->gravity.z);
}

float imu_heading(struct position2 *pos)
{
	struct vect forward;

	forward.x = 0;
	forward.y = 1;
	forward.z = 0;

	rotate_vector(&pos->forward, &forward, &pos->orientation);

	return atan2f(pos->forward.x, pos->forward.y);
}

void update_wheel_coord_imu(struct position2 *pos)
{
	rotate_vector(&pos->world_wheel_coord_imu, &pos->local_wheel_coord_imu,
		&pos->orientation);
}

static void update_gravity_vector(struct position2 *pos)
{
	/*pos->gravity.x = 2 * (pos->orientation.x * pos->orientation.z -
		pos->orientation.w * pos->orientation.y); 
	pos->gravity.y = 2 * (pos->orientation.w * pos->orientation.x +
		pos->orientation.y * pos->orientation.z); 
	pos->gravity.z = pos->orientation.w * pos->orientation.w -
		pos->orientation.x * pos->orientation.x -
			pos->orientation.y * pos->orientation.y +
				pos->orientation.z * pos->orientation.z;*/

	struct vect down;
	down.x = 0;
	down.y = 0;
	down.z = 1;

	struct quaternion heading, temp;

	heading.x = 0;
	heading.y = 0;
	heading.z = sinf(pos->heading / 2);
	heading.x = cosf(pos->heading / 2);

	quaternion_product(&temp, &heading, &pos->orientation);

	rotate_vector(&pos->gravity, &down, &temp);

	//printf("quat:	%f, %f, %f\n", pos->gravity.x, pos->gravity.y, pos->gravity.z);
}

void update_wheel_position(struct position2 *pos)
{
	float tilt_encoder_offset = 0;//-sinf(pos->tilt) * (720 / M_PI);
  //printf("%f\n", tilt_encoder_offset);

	float left_movement = pos->left - pos->last_left + tilt_encoder_offset;
	pos->last_left = pos->left + tilt_encoder_offset;

	float right_movement = pos->right - pos->last_right + tilt_encoder_offset;
	pos->last_right = pos->right + tilt_encoder_offset;

	pos->movement = ((left_movement + right_movement) / 2.0);

	pos->distance_moved += pos->movement; 

	//printf("%f\n", pos->tilt);

	pos->movement = pos->movement * 0.349065850398865;

	pos->wheel_heading_change = ((right_movement - left_movement) *
			 0.349065850398865) / pos->wheel_base;
}

void update_imu_position(struct position2 *pos)	
{
	update_gravity_vector(pos);

	float heading = imu_heading(pos);

	pos->local_accels.x = pos->accels.x - pos->gravity.x * 8192.0;
	pos->local_accels.y = pos->accels.y - pos->gravity.y * 8192.0;
	pos->local_accels.z = pos->accels.z - pos->gravity.z * 8192.0;

	rotate_vector(&pos->world_accels, &pos->local_accels, &pos->orientation);

	pos->imu_velocity.x += ((pos->world_accels.x * G) / 8192.0) * .005;
	pos->imu_velocity.y += ((pos->world_accels.y * G) / 8192.0) * .005;
	pos->imu_velocity.z += ((pos->world_accels.z * G) / 8192.0) * .005;

	pos->imu_position.x += ((pos->imu_velocity.x) * .005);
	pos->imu_position.y += ((pos->imu_velocity.y) * .005);
	pos->imu_position.z += ((pos->imu_velocity.z) * .005);

	//printf("%f, %f, %f\n", pos->imu_position.x, pos->imu_position.y, pos->imu_position.z);
}
