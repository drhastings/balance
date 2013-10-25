#include "../includes/pd.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct pd *new_pd(float P, float D, int num_samples, float max, float min, float *o,
										float *t)
{
	struct pd * pd;

	pd = (struct pd *)malloc(sizeof(struct pd));

	if (!pd)
		return NULL;

	pd->P = P;

	pd->D = D;

	pd->num_samples = num_samples;

	pd->sample_index = 0;

	pd->last_error = malloc(sizeof(float) * num_samples);

	memset(pd->last_error, 0 , sizeof(float) * num_samples);

	pd->max = max;

	pd->min = min;

	pd->observation = o;

	pd->target = t;

	return pd;
}

void free_pd(struct pd *pd)
{
	free(pd);
}

float pd_loop(struct pd *pd)
{
	float ret = 0;
	float error = *pd->observation - *pd->target;

	if (fabs(error) < pd->min)
		error = 0;

	if (pd->max && fabs(error) > pd->max)
		return pd->max * (fabs(ret) / ret);

	ret += error * pd->P;

	if (pd->sample_index == pd->num_samples - 1)
	{
		ret += (error - pd->last_error[0]) * pd->D;
	}
	else
	{
		ret += (error - pd->last_error[pd->sample_index + 1]) * pd->D;
	}
	
	pd->last_error[pd->sample_index] = error;

	pd->sample_index++;

	if (pd->sample_index == pd->num_samples)
	{
		pd->sample_index = 0;
	}

	return ret;
}
