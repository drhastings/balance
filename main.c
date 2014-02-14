#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/timerfd.h>

#include <pigpio.h>

#include "includes/quaternion.h"
#include "includes/state.h"
#include "includes/surface.h"
#include "includes/ogl.h"
#include "includes/matrix.h"
#include "includes/position.h"
#include "includes/pd.h"
#include "includes/motor.h"
#include "includes/robot.h"
#include "includes/server.h"
#include "includes/node.h"
#include "includes/page.h"
#include "includes/coord.h"
#include "includes/fly.h"
#include "includes/text.h"
#include "includes/variable.h"
#include "includes/strings.h"
#include "includes/control_panel.h"
#include "includes/turret.h"
#include "includes/list.h"

#include "includes/line_detect.h"

#define  ENCODER_FILE "/dev/encoders"

static struct robot robot;

CUBE_STATE_T state, *p_state = &state;

void time_diff(struct timeval *now, struct timeval *then, struct timeval *out);

void *range_read(void *robot)
{
  struct robot *bot = (struct robot *) robot;
  int range_file;
  char data[7];

  memset(data, 0, 7);

  range_file = open("/dev/ttyAMA0", O_RDONLY);

  if (!range_file)
    printf("no range info\n");

  while(1)
  {
    read(range_file, data, 6);
  
//    printf("%f\n", bot->range -45.0);

    bot->range = atof(&data[1]);
  }

  return NULL;
}

int encode_loop(CUBE_STATE_T *state, struct robot *robot);

void *render_task(void *dummy)
{
  init_ogl(p_state);

  create_perspective_matrix(&p_state->p_matrix, 45, 1.0, 0.1, 100);

  identity(&p_state->mv_matrix);
  identity(&p_state->send_mv_matrix);

  rotate_matrix(&p_state->send_mv_matrix, 180, 1.0, 0, 0);
  translate_matrix(&p_state->send_mv_matrix, 0, 0, -5);
  //rotate_matrix(&p_state->mv_matrix, 45, 1.0, 0, 0);
  translate_matrix(&p_state->mv_matrix, 0, -10, -15);

  init_shaders(p_state);

  init_framebuffer(p_state);

  init_textures(p_state);

  encode_loop(p_state, &robot);

  return NULL;
}

void dump_matrix(struct matrix *mat)
{
  printf("%f, %f, %f, %f\n", mat->elements[0][0], mat->elements[1][0],mat->elements[2][0],mat->elements[3][0]);
  printf("%f, %f, %f, %f\n", mat->elements[0][1], mat->elements[1][1],mat->elements[2][1],mat->elements[3][1]);
  printf("%f, %f, %f, %f\n", mat->elements[0][2], mat->elements[1][2],mat->elements[2][2],mat->elements[3][2]);
  printf("%f, %f, %f, %f\n", mat->elements[0][3], mat->elements[1][3],mat->elements[2][3],mat->elements[3][3]);
}

void set_thread_priority(pthread_t thread, int rank)
{
  struct sched_param params;

  params.sched_priority = sched_get_priority_max(SCHED_FIFO) - rank;

  pthread_setschedparam(thread, SCHED_FIFO, &params);
}

int main(int argc, char** argv)
{
  init_robot(&robot);

  struct coord *destination;

  destination = new_coord(0, 0);

  destination->x = 0;
  destination->y = 0;

  pthread_mutex_init(&state.frame_lock, NULL);
  pthread_cond_init(&state.has_frame, NULL);

  robot.dest = destination;

  gpioSetMode(4, PI_OUTPUT);
  gpioSetMode(24, PI_OUTPUT);
  gpioSetMode(5, PI_OUTPUT);
  gpioSetMode(14, PI_OUTPUT);

  //870 for at the ground, 

//  gpioServo(24,1380);
//  gpioServo(4, 2100);

  pthread_t range_thread, render_thread, cv_thread;

  pthread_create(&range_thread, NULL, &range_read, &robot);
  pthread_create(&render_thread, NULL, &render_task, NULL);
//  pthread_create(&cv_thread, NULL, &find_circles, &state);

  gpioWrite(5, 0);
  gpioWrite(14, 0);

  start_control_panel(&robot, &state);

  pthread_t this_thread = pthread_self();

  //set_thread_priority(this_thread, 0);
  //set_thread_priority(robot.server->serve_thread, 1);
  //set_thread_priority(range_thread, 2);

  read(robot.encoder_handle, robot.sensor_data, sizeof(robot.sensor_data));

  getQuaternion(&robot.position.orientation, robot.sensor_data);

  robot.position.last_left = *((int *) (robot.sensor_data + 42));
  robot.position.last_right = *((int *) (robot.sensor_data + 46));

  struct robot_task *position_task, *point_task, *remote_task,
    *spin_task, *move_task, *adjust_task, *stand_up_task, *fly_task,
      *avoid_task, *stay_task;

  position_task = (struct robot_task *)malloc(sizeof(*position_task));

  position_task->task_func = &update_position2;

  fly_task = (struct robot_task *)malloc(sizeof(*fly_task));

  fly_task->task_func = &buzz;

  point_task = (struct robot_task *)malloc(sizeof(*point_task));

  robot.turret.target.x = -10000;
  robot.turret.target.y = 10000;
  robot.turret.target.z = 0;

  point_task->task_func = &follow;
  

  remote_task = (struct robot_task *)malloc(sizeof(*remote_task));

  remote_task->task_func = &remote;

  avoid_task = (struct robot_task *)malloc(sizeof(*avoid_task));

  avoid_task->task_func = &stay_away;

  stay_task = (struct robot_task *)malloc(sizeof(*stay_task));

  stay_task->task_func = &stay_within;

  move_task = (struct robot_task *)malloc(sizeof(*move_task));

  move_task->task_func = &move;

  adjust_task = (struct robot_task *)malloc(sizeof(*adjust_task));

  adjust_task->task_func = &adjust_speeds;

  stand_up_task = (struct robot_task *)malloc(sizeof(*stand_up_task));

  stand_up_task->task_func = &stand_up;

  INIT_LIST_HEAD(&position_task->list_item);
  INIT_LIST_HEAD(&fly_task->list_item);
  INIT_LIST_HEAD(&point_task->list_item);
  INIT_LIST_HEAD(&remote_task->list_item);
  INIT_LIST_HEAD(&avoid_task->list_item);
  INIT_LIST_HEAD(&stay_task->list_item);
  INIT_LIST_HEAD(&move_task->list_item);
  INIT_LIST_HEAD(&adjust_task->list_item);
  INIT_LIST_HEAD(&stand_up_task->list_item);

  list_add_tail(&position_task->list_item, &robot.task_list);
//  list_add_tail(&fly_task->list_item, &robot.task_list);
  list_add_tail(&point_task->list_item, &robot.task_list);
  list_add_tail(&remote_task->list_item, &robot.task_list);
//  list_add_tail(&avoid_task->list_item, &robot.task_list);
//  list_add_tail(&stay_task->list_item, &robot.task_list);
//  list_add_tail(&move_task->list_item, &robot.task_list);
  list_add_tail(&adjust_task->list_item, &robot.task_list);
  list_add_tail(&stand_up_task->list_item, &robot.task_list);

  get_sensor_data(&robot.position2, robot.sensor_data);

  init_position(&robot.position2);
  
  while(1)
  {
    read(robot.encoder_handle, robot.sensor_data, sizeof(robot.sensor_data));

    get_sensor_data(&robot.position2, robot.sensor_data);

//    fprintf(stderr, "%i, %i\n", robot.position2.left, robot.position2.right);

    if (!((robot.position2.orientation.x == robot.position2.orientation.y) &&
      (robot.position2.orientation.x == robot.position2.orientation.z) &&
      (robot.position2.orientation.x == robot.position2.orientation.w)))
    {
      do_robot_tasks(&robot);
    }

    state.roll = robot.turret.roll * 180 / M_PI;

    if (state.x_pos < .5)
    {
      robot.turret.yaw += 0.001;
    }
    if (state.x_pos > .5)
    {
      robot.turret.yaw -= 0.001;
    }
    if (state.y_pos < .5)
    {
      robot.turret.pitch += 0.001;
    } 
    if (state.y_pos > .5)
    {
      robot.turret.pitch -= 0.001;
    }  
  
    struct timeval now, then;

//    printf("%f, %f, %f, %f, %f, %f\n", robot.position2.position.x, robot.position2.position.y, robot.turret.target.x, robot.turret.target.y, robot.dest->x, robot.dest->y);

    gettimeofday(&now, NULL);    

/*    fprintf(stderr, "%ld.%06ld, %f, %f, %f, %f, %i, %i, %f\n", now.tv_sec, now.tv_usec,
      robot.position2.orientation.x, robot.position2.orientation.y,
        robot.position2.orientation.z, robot.position2.orientation.w, 
      robot.position2.left, robot.position2.right, robot.position2.heading);
*/
    //printf("%f\n", robot.range);

    //printf("%f, %f, %f\n", robot.position2.position.x, robot.position2.position.y, robot.position2.tilt);

//    fprintf(stderr, "%f, %f, %f, %i, %i\n", robot.position2.position.x, robot.position2.position.y, robot.position2.heading, robot.position2.left, robot.position2.right);
  }
}

void time_diff(struct timeval *now, struct timeval *then, struct timeval *out)
{
  long u_diff = 0, s_diff = 0;

  u_diff = now->tv_usec - then->tv_usec;
  s_diff = now->tv_sec - then->tv_sec;

  if (u_diff < 0)
  {
    s_diff--;

    u_diff += 1000000;
  }

  out->tv_sec = s_diff;

  out->tv_usec = u_diff;
}
