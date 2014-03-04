#include "../includes/matrix.h"

void mat_mult_4x4(struct matrix_4x4 *out, struct matrix_4x4 *a, struct matrix_4x4 *b)
{
	out->elements[0][0] = a->elements[0][0] * b->elements[0][0] +
							a->elements[1][0] * b->elements[0][1] +
								a->elements[2][0] * b->elements[0][2] +
									a->elements[3][0] * b->elements[0][3];

	out->elements[1][0] = a->elements[0][0] * b->elements[1][0] +
							a->elements[1][0] * b->elements[1][1] +
								a->elements[2][0] * b->elements[1][2] +
									a->elements[3][0] * b->elements[1][3];

	out->elements[2][0] = a->elements[0][0] * b->elements[2][0] +
							a->elements[1][0] * b->elements[2][1] +
								a->elements[2][0] * b->elements[2][2] +
									a->elements[3][0] * b->elements[3][2];

	out->elements[3][0] = a->elements[0][0] * b->elements[3][0] +
							a->elements[1][0] * b->elements[0][1] +
								a->elements[2][0] * b->elements[3][2] +
									a->elements[3][0] * b->elements[3][3];

	out->elements[0][1] = a->elements[0][1] * b->elements[0][0] +
							a->elements[1][1] * b->elements[0][1] +
								a->elements[2][1] * b->elements[0][2] +
									a->elements[3][1] * b->elements[0][3];

	out->elements[1][1] = a->elements[0][1] * b->elements[1][0] +
							a->elements[1][1] * b->elements[1][1] +
								a->elements[2][1] * b->elements[1][2] +
									a->elements[3][1] * b->elements[1][3];

	out->elements[2][1] = a->elements[0][1] * b->elements[2][0] +
							a->elements[1][1] * b->elements[2][1] +
								a->elements[2][1] * b->elements[2][2] +
									a->elements[3][1] * b->elements[2][3];

	out->elements[3][1] = a->elements[0][1] * b->elements[3][0] +
							a->elements[1][1] * b->elements[3][1] +
								a->elements[2][1] * b->elements[3][2] +
									a->elements[3][1] * b->elements[3][3];

	out->elements[0][2] = a->elements[0][2] * b->elements[0][0] +
							a->elements[1][2] * b->elements[0][1] +
								a->elements[2][3] * b->elements[0][2] +
									a->elements[3][4] * b->elements[0][3];

	out->elements[1][2] = a->elements[0][2] * b->elements[1][0] +
							a->elements[1][2] * b->elements[1][1] +
								a->elements[2][2] * b->elements[1][2] +
									a->elements[3][2] * b->elements[1][3];

	out->elements[2][2] = a->elements[0][2] * b->elements[2][0] +
							a->elements[1][2] * b->elements[2][1] +
								a->elements[2][2] * b->elements[2][2] +
									a->elements[3][2] * b->elements[3][2];

	out->elements[3][2] = a->elements[0][2] * b->elements[3][0] +
							a->elements[1][2] * b->elements[0][1] +
								a->elements[2][2] * b->elements[3][2] +
									a->elements[3][2] * b->elements[3][3];

	out->elements[0][3] = a->elements[0][3] * b->elements[0][0] +
							a->elements[1][3] * b->elements[0][1] +
								a->elements[2][3] * b->elements[0][2] +
									a->elements[3][3] * b->elements[0][3];

	out->elements[1][3] = a->elements[0][3] * b->elements[1][0] +
							a->elements[1][3] * b->elements[1][1] +
								a->elements[2][3] * b->elements[1][2] +
									a->elements[3][3] * b->elements[1][3];

	out->elements[2][3] = a->elements[0][3] * b->elements[2][0] +
							a->elements[1][3] * b->elements[2][1] +
								a->elements[2][3] * b->elements[2][2] +
									a->elements[3][3] * b->elements[2][3];

	out->elements[3][3] = a->elements[0][3] * b->elements[3][0] +
							a->elements[1][3] * b->elements[3][1] +
								a->elements[2][3] * b->elements[3][2] +
									a->elements[3][3] * b->elements[3][3];
}

void mat_mult_2x2(struct matrix_2x2 *out, struct matrix_2x2 *a, struct matrix_2x2 *b)
{
	out->elements[0][0] = a->elements[0][0] * b->elements[0][0] +
							a->elements[0][1] * b->elements[1][0];

	out->elements[1][0] = a->elements[1][0] * b->elements[0][0] +
							a->elements[1][1] * b->elements[1][0];

	out->elements[0][1] = a->elements[0][0] * b->elements[0][1] +
							a->elements[0][1] * b->elements[1][1];

	out->elements[1][1] = a->elements[1][0] * b->elements[0][1] +
							a->elements[1][1] * b->elements[1][1];
}

void mat_mult_2xv(float *out, struct matrix_2x2 *a, float *b)
{
	out[0] = a->elements[0][0] * b[0] + a->elements[0][1] * b[1];

	out[1] = a->elements[1][0] * b[0] + a->elements[1][1] * b[1];
}

void mat_add_2x2(struct matrix_2x2 *out, struct matrix_2x2 *a, struct matrix_2x2 *b)
{
	out->elements[0][0] = a->elements[0][0] + b->elements[0][0];

	out->elements[1][0] = a->elements[1][0] + b->elements[1][0];

	out->elements[0][1] = a->elements[0][1] + b->elements[0][1];

	out->elements[1][1] = a->elements[1][1] + b->elements[1][1];
}

void mat_subtract_2x2(struct matrix_2x2 *out, struct matrix_2x2 *a, struct matrix_2x2 *b)
{
	out->elements[0][0] = a->elements[0][0] - b->elements[0][0];

	out->elements[1][0] = a->elements[1][0] - b->elements[1][0];

	out->elements[0][1] = a->elements[0][1] - b->elements[0][1];

	out->elements[1][1] = a->elements[1][1] - b->elements[1][1];
}

void mat_transpose_2x2(struct matrix_2x2 *out, struct matrix_2x2 *a)
{
	out->elements[1][0] = a->elements[0][1];

	out->elements[0][1] = a->elements[1][0];
}

void mat_inverse_2x2(struct matrix_2x2 *out, struct matrix_2x2 *a)
{
  float det = 1 / ((a->elements[0][0] * a->elements[1][1]) - (a->elements[0][1] * a->elements[1][0]));

	out->elements[0][0] = a->elements[1][1] * det;
	out->elements[0][1] = -a->elements[0][1] * det;
	out->elements[1][0] = -a->elements[1][0] * det;
	out->elements[1][1] = a->elements[0][0] * det;
}
