#ifndef OGL__H
#define OGL__H

#include "state.h"
#include "matrix.h"

void *find_circles(void *input);

void init_ogl(CUBE_STATE_T *state);

void init_shaders(CUBE_STATE_T *state);

void init_framebuffer(CUBE_STATE_T *state);

void init_textures(CUBE_STATE_T *state);

void create_perspective_matrix(struct matrix_4x4 *out, float fovy, float aspect,
							float zn, float zf);

void rotate_matrix(struct matrix_4x4 *mat, float angle, float x, float y, float z);

void translate_matrix(struct matrix_4x4 *mat, float x, float y, float z);

void identity(struct matrix_4x4 *mat);

void redraw_scene(CUBE_STATE_T *state);

#endif
