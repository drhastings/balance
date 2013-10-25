#ifndef PD__H
#define PD__H

struct pd
{
	float P;

	float D;

	float num_samples;

	int sample_index;

	float *last_error;

	float max;

	float min;

	float *observation;

	float *target;
};

struct pd *new_pd(float P, float D, int num_samples, float max, float min,
									float *o, float *t);

void free_pd(struct pd *pd);

float pd_loop(struct pd *pd);

#endif
