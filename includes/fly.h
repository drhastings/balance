#ifndef __FLY_H
#define __FLY_H

#include "vector.h"

struct robot;

struct fly
{
  struct vect position;

  float heading;

  float speed;

  int flight_counter;

  float distance;
};

void init_fly(struct fly *fly);

void get_acceleration(struct fly *fly);

void accelerate(struct fly *fly);

void move_fly(struct fly *fly);

void takeoff(struct fly *fly);

void buzz(struct robot *robot);

#endif

