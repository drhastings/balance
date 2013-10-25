#include "../includes/quaternion.h"
#include <math.h>

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

	ret = asinf(ret);

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
	result->y = p->w * q->y - p->x * q->z + p->y * q->w + p->z * q->x;
	result->z = p->w * q->z + p->x * q->y - p->y * q->x + p->z * q->w;
}

void get_conjugate(struct quaternion *result, struct quaternion *q)
{
	result->w = q->w;
	result->x = -q->x;
	result->y = -q->y;
	result->z = -q->z;
}
