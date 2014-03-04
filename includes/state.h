#ifndef __STATE_H
#define __STATE_H

#include "GLES/gl.h"
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "matrix.h"

typedef struct
{
   uint32_t screen_width;
   uint32_t screen_height;
// OpenGL|ES objects
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;
   GLuint tex;
   GLuint render_texture;
   GLuint texture_512;
   GLuint texture_128;
   GLuint texture_32;
   GLuint texture_8;
   GLuint texture_2;
   GLuint texture_1;
	 GLuint offscreen_renderbuffer;
	 GLuint y_framebuffer;
	 GLuint u_framebuffer;
	 GLuint v_framebuffer;
	 GLuint framebuffer_512;
	 GLuint framebuffer_128;
	 GLuint framebuffer_32;
	 GLuint framebuffer_8;
	 GLuint framebuffer_2;
	 GLuint framebuffer_1;
	 GLuint render_program;
	 GLuint quarter_program;
	 GLuint half_program;
	 GLuint mask_program;
	 GLuint y_program;
	 GLuint u_program;
	 GLuint v_program;

	 GLuint attr_vertex;
	 GLuint attr_tex;
	 GLuint unif_tex;
	 GLuint mask_tex;
	 GLuint quarter_tex;
	 GLuint half_tex;
	 GLuint y_tex;
	 GLuint u_tex;
	 GLuint v_tex;
	 GLuint unif_pmatrix;
	 GLuint quarter_pmatrix;
	 GLuint half_pmatrix;
	 GLuint mask_pmatrix;
	 GLuint y_pmatrix;
	 GLuint u_pmatrix;
	 GLuint v_pmatrix;
	 GLuint unif_mvmatrix;
	 GLuint mask_mvmatrix;
	 GLuint quarter_mvmatrix;
	 GLuint half_mvmatrix;
	 GLuint y_mvmatrix;
	 GLuint u_mvmatrix;
	 GLuint v_mvmatrix;

   GLuint target_color; 
   GLuint max_distance;
   GLuint invs_size;

   GLfloat color[2];  
   GLfloat distance;  

   float x_pos;
   float y_pos;

	 void *eglImage;

	 GLuint out_buffers[3];

	 GLuint dump_frame;

	 struct matrix_4x4 p_matrix;
	 struct matrix_4x4 mv_matrix;
	 struct matrix_4x4 send_mv_matrix;

	 char *write_buffer;

	 float roll;

   pthread_mutex_t frame_lock;

   pthread_cond_t has_frame;

   int need_frame;

    float v_min, v_max, u_min, u_max;


   uint8_t frame[160 * 640 * 4 + 2 * (80 * 320 * 4)];

   int set_target;
} CUBE_STATE_T;

#endif
