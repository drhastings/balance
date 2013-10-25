#include "../includes/turret.h"
#include "../includes/vector.h"
#include <math.h>
#include <stdio.h>
#include <pigpio.h>

void pan_tilt_to_pitch_roll(struct turret *turret)
{
	struct vect target_vector;

	float xylen = cosf(turret->tilt * M_PI / 180.0);

	target_vector.x = xylen * cosf(turret->pan * M_PI / 180.0);
	target_vector.z = sin(turret->tilt * M_PI / 180.0);
	target_vector.y = xylen * sin(-turret->pan * M_PI / 180.0);

	//printf("%f, %f, %f\n", target_vector.x, target_vector.y, target_vector.z);

	turret->pitch = asinf(target_vector.x) * 180.0 / M_PI;
	float cos_pitch = sqrtf(1 - (target_vector.x * target_vector.x));

	if (cos_pitch == 0 || fabs(target_vector.x >= 1))
	{
		turret->roll = 90;
	}
	else
	{
		turret->roll = fabs(atan2f(target_vector.z, target_vector.y)) * 180.0 / M_PI;
	}

	float conversion_factor = 1000.0 / 90.0;

	float pitch_setting = turret->pitch * conversion_factor + 700;
	float roll_setting = turret->roll * conversion_factor + 380;

	//printf("%f\n", roll_setting);

	if (pitch_setting > 2000) pitch_setting = 2000;
	if (pitch_setting < 800) pitch_setting = 800;
	if (roll_setting > 2300) roll_setting = 2300;
	if (roll_setting < 500) roll_setting = 500;

	//printf("%f\n", roll_setting);
	gpioServo(24,roll_setting);
	gpioServo(4, pitch_setting);


	//printf("%f, %f\n", turret->pitch, turret->roll);
}
