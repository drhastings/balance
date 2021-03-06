#include "../includes/quaternion.h"
#include <math.h>
#include <stdio.h>
#include "../includes/robot.h"

int getQuaternion(struct quaternion *output, char *packet)
{
	int16_t temp[4];

	temp[0] = ((packet[0] << 8) + packet[1]);
	temp[1] = ((packet[4] << 8) + packet[5]);
	temp[2] = ((packet[8] << 8) + packet[9]);
	temp[3] = ((packet[12] << 8) + packet[13]);

	output->w = (float)temp[0] / 16384.0f;
	output->x = (float)temp[1] / 16384.0f;
	output->y = (float)temp[2] / 16384.0f;
	output->z = (float)temp[3] / 16384.0f;

	return 0;
}

float get_tilt_angle(struct robot *robot)
{
	struct vect down, out;

	down.x = 0;
	down.y = 0;
	down.z = -1;

	rotate_vector(&out, &down, &robot->position2.orientation);

	//printf("%f, %f, %f\n", out.x, out.y, out.z);

	return atan2f(out.z, out.y);
}

float quaternion_to_tilt(struct quaternion *quat)
{
	struct vect gravity;

	gravity.x = 2 * (quat->x * quat->z - quat->w * quat->y);

	gravity.y = 2 * (quat->w * quat->x + quat->y * quat->z);

	gravity.z = quat->w * quat->w - quat->x * quat->x - 
					quat->y * quat->y + quat->z * quat->z;

	quat->gravity = gravity;

	float ret = gravity.z;

	if (ret > 1) ret = 1;
	if (ret < -1) ret = -1;

	//printf("matrix:	%f, %f, %f\n", gravity.x, gravity.y, gravity.z);

	ret = -atan2(-gravity.z, -gravity.y);

	return ret;
}

float quaternion_to_theta(struct quaternion *quat)
{
	struct vect temp;

	temp.x = 2 * (quat->x * quat->y + quat->w * quat->z);

	temp.y = quat->w * quat->w - quat->x * quat->x + 
					quat->y * quat->y - quat->z * quat->z;

	temp.z = 2 * (quat->y * quat->z - quat->w * quat->x);

	return atan2f(-temp.x, -temp.z);
}

void quaternion_product(struct quaternion *result, struct quaternion * p,
			struct quaternion *q)
{
	result->w = p->w * q->w - p->x * q->x - p->y * q->y - p->z * q->z;
	result->x = p->w * q->x + p->x * q->w + p->y * q->z - p->z * q->y;
	result->y = p->w * q->y + p->y * q->w + p->z * q->x - p->x * q->z;
	result->z = p->w * q->z + p->z * q->w + p->x * q->y - p->y * q->x;
}

void get_conjugate(struct quaternion *result, struct quaternion *q)
{
	result->w = q->w;
	result->x = -q->x;
	result->y = -q->y;
	result->z = -q->z;
}

void rotate_vector(struct vect *out, struct vect *in, struct quaternion *r)
{
	struct quaternion temp, sum, res, inverse;

	temp.x = in->x;
	temp.y = in->y;
	temp.z = in->z;
	temp.w = 0;

	get_conjugate(&inverse, r);

	quaternion_product(&sum, r, &temp);

	quaternion_product(&res, &sum, &inverse);

	out->x = res.x;
	out->y = res.y;
	out->z = res.z;
}
