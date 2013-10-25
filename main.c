#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <signal.h>

#include <pigpio.h>

#include "includes/quaternion.h"
#include "includes/position.h"
#include "includes/pd.h"
#include "includes/motor.h"
#include "includes/robot.h"
#include "includes/server.h"
#include "includes/node.h"
#include "includes/page.h"
#include "includes/coord.h"
#include "includes/text.h"
#include "includes/variable.h"
#include "includes/strings.h"
#include "includes/control_panel.h"
#include "includes/turret.h"

#include "includes/line_detect.h"

#define  ENCODER_FILE "/dev/encoders"

static struct robot robot;

void *range_read(void *robot)
{
	struct robot *bot = (struct robot *) robot;
	int range_file;
	char data[7];

	memset(data, 0, 7);

	range_file = open("/dev/ttyAMA0", O_RDONLY);

	if (!range_file)
		printf("no range info\n");

	while(1)
	{
		read(range_file, data, 6);
	
		bot->range = atof(&data[1]);
	}

	return NULL;
}

int main(int argc, char** argv)
{
	init_robot(&robot);

	gpioSetMode(4, PI_OUTPUT);
	gpioSetMode(24, PI_OUTPUT);
	gpioSetMode(5, PI_OUTPUT);
	gpioSetMode(14, PI_OUTPUT);

	//870 for at the ground, 

//	gpioServo(24,1380);
//	gpioServo(4, 2100);

//	pthread_t detect_thread;
	pthread_t range_thread;

//	pthread_create(&detect_thread, NULL, &detect, NULL);
	pthread_create(&range_thread, NULL, &range_read, &robot);

	gpioWrite(5, 0);
	gpioWrite(14, 1);

	start_control_panel(&robot);

	read(robot.encoder_handle, robot.sensor_data, sizeof(robot.sensor_data));

	getQuaternion(&robot.position.orientation, robot.sensor_data);

	robot.position.last_left = *((int *) (robot.sensor_data + 42));
	robot.position.last_right = *((int *) (robot.sensor_data + 46));

	while(1)
	{
		read(robot.encoder_handle, robot.sensor_data, sizeof(robot.sensor_data));

		//gpioServo(24,robot.turret.roll);
		//gpioServo(4, robot.turret.pitch);

		getQuaternion(&robot.position.orientation, robot.sensor_data);

		robot.left_count = *((int *) (robot.sensor_data + 42));
		robot.right_count = *((int *) (robot.sensor_data + 46));

		robot.position.tilt = quaternion_to_tilt(&robot.position.orientation);

		getAccels(&robot.position, robot.sensor_data);

		robot.position.inertial_heading = quaternion_to_theta(&robot.position.orientation);

		float x = 0, y = 0;

		float camera_x = robot.position.x_inertial + 30 * cosf(robot.position.incr_inertial_heading);
		float camera_y = robot.position.y_inertial + 30 * sinf(robot.position.incr_inertial_heading);

		if (robot.dest)
		{
			float look_ahead = stopping_distance(robot.linear_velocity,
					robot.linear_acceleration);

			float look_angle = angle_to(&robot, camera_x, camera_y, robot.dest->x, robot.dest->y);

			if (look_ahead < 500) look_ahead = 500;

			x = camera_x + look_ahead * cosf(robot.position.incr_inertial_heading + look_angle);
			y = camera_y + look_ahead * sinf(robot.position.incr_inertial_heading + look_angle);
		}
		else
		{
			float look_ahead = stopping_distance(robot.linear_velocity,
					robot.linear_acceleration);

			float time_to_stop = fabs(robot.linear_velocity
				/	robot.linear_acceleration);

			float look_angle = 0;//time_to_stop * robot.rotational_velocity;

			if (look_ahead < 500) look_ahead = 500;

			x = camera_x + look_ahead * cosf(robot.position.incr_inertial_heading + look_angle);
			y = camera_y + look_ahead * sinf(robot.position.incr_inertial_heading + look_angle);
		}

		//printf("%f\n", robot.range);

		robot.turret.pan = angle_to(&robot, camera_x, camera_y, x, y) * 180 / M_PI;

		float local_tilt = robot.position.tilt - robot.tilt_target;

		float wanted_tilt = atan2f(cosf(local_tilt) * 230 + 40, 
			distance_to(camera_x, camera_y, x, y) -
			 (sinf(local_tilt) * 230));

		wanted_tilt = -((wanted_tilt - local_tilt) * (180 / M_PI));

		robot.turret.tilt = wanted_tilt;

		//printf("%f %f\n", wanted_tilt, robot.position.tilt *  (180 / M_PI));

		if (robot.state == STATE_BALANCE)
		{
			update_position(&robot.position, robot.left_count, robot.right_count);
			pan_tilt_to_pitch_roll(&robot.turret);
		}
		else if (robot.state == STATE_CALIBRATING)
		{

		}
		if (fabs(robot.position.tilt - robot.tilt_target) > 0.35)
		{
			robot.position.incr_inertial_heading = 0;
	
			robot.position.heading = robot.position.incr_inertial_heading; 

			robot.rotational_target = robot.position.heading;
			robot.linear_target = robot.position.linear_count;

			robot.position.x = 0;
			robot.position.y = 0;
			robot.position.x_inertial = 0;
			robot.position.y_inertial = 0;

			robot.is_following = 0;
		}

		robot.rotational_velocity_setpoint = -robot.in_spin * robot.rotational_velocity_max;

		robot.linear_velocity_setpoint = robot.in_speed * robot.linear_velocity_max;

		move(&robot);

		float tilt_output = pd_loop(robot.tilt_loop);

		float linear_output = pd_loop(robot.linear_loop);

/*		if (tilt_output * linear_output < 0)
		{
			if (tilt_output > 0)
				robot.tilt_target += .0000001;
			else if (tilt_output < 0)
				robot.tilt_target -= .0000001;
		}*/

		float spin_output = pd_loop(robot.spin_loop);

		if (robot.position.has_traction)
		{
			set_motor(&robot.left_motor, tilt_output + linear_output - spin_output);
			set_motor(&robot.right_motor, tilt_output + linear_output + spin_output);
		}
		else
		{
			set_motor(&robot.left_motor, 0);
			set_motor(&robot.right_motor, 0);
		}
	}
}
