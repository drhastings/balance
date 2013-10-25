#ifndef MOTOR__H
#define MOTOR__H

struct motor
{
	int pin_a;

	int pin_b;

	int enable;

	float min;

	float boost;
};

void init_motor(struct motor *motor);

void set_motor(struct motor *motor, float percent_power);

#endif
