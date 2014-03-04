#ifndef __MATRIX_H
#define __MATRIX_H

#include "GLES/gl.h"
#include "GLES2/gl2.h"

struct matrix_4x4
{
	GLfloat elements[4][4];
};

struct matrix_2x2
{
	float elements[2][2];
};

void mat_mult_4x4(struct matrix_4x4 *out, struct matrix_4x4 *a, struct matrix_4x4 *b);

void mat_mult_2x2(struct matrix_2x2 *out, struct matrix_2x2 *a, struct matrix_2x2 *b);

void mat_mult_2xv(float *out, struct matrix_2x2 *a, float *b);

void mat_add_2x2(struct matrix_2x2 *out, struct matrix_2x2 *a, struct matrix_2x2 *b);

void mat_subtract_2x2(struct matrix_2x2 *out, struct matrix_2x2 *a, struct matrix_2x2 *b);

void mat_transpose_2x2(struct matrix_2x2 *out, struct matrix_2x2 *a);

void mat_inverse_2x2(struct matrix_2x2 *out, struct matrix_2x2 *a);

#endif
