#include "../includes/position.h"
#include "../includes/quaternion.h"
#include <stdio.h>

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

	return 0;
}	
