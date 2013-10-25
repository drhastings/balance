#include "../includes/motor.h"
#include <pigpio.h>
#include <math.h>
#include <stdio.h>

void init_motor(struct motor *motor)
{
	gpioSetMode(motor->pin_a, PI_OUTPUT);
	gpioSetMode(motor->pin_b, PI_OUTPUT);
	gpioSetMode(motor->enable, PI_OUTPUT);

	gpioSetPWMfrequency(motor->enable, 400);

	gpioSetPWMrange(motor->enable, 1000);
}

void set_motor(struct motor *motor, float percent_power)
{
	if (percent_power > .6) percent_power = .6;
	if (percent_power < -.6) percent_power = -.6;

	float power = (1000 * fabs(percent_power)) + motor->boost;

//	if (power < motor->min)
//		power = 0;

	if (power > 1000)
		power = 1000;

//	printf("%f\n", power);

	if (percent_power > 0)
	{
		gpioWrite(motor->pin_a, 0);
		gpioWrite(motor->pin_b, 1);
		gpioPWM(motor->enable, power);
	}
	if (percent_power < 0)
	{
		gpioWrite(motor->pin_b, 0);
		gpioWrite(motor->pin_a, 1);
		gpioPWM(motor->enable, power);
	}
	if (percent_power == 0)
	{
		gpioWrite(motor->pin_a, 0);
		gpioWrite(motor->pin_b, 0);
		gpioPWM(motor->enable, 1000);
	}
}
