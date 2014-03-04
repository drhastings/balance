#include "../includes/fly.h"
#include "../includes/robot.h"
#include "../includes/coord.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void init_fly(struct fly *fly)
{
  fly->position.x = 500;
  fly->position.y = 0;
  fly->position.z = -40;

  fly->heading = 0;
  fly->speed = 4;

  fly->distance = 0;

  fly->flight_counter = 0;

  srand(time(NULL));
}

void get_acceleration(struct fly *fly);

void accelerate(struct fly *fly);

void move_fly(struct robot *robot, struct fly *fly)
{
  static float angle = 0;

  fly->position.x = (cosf(angle) * 700) + robot->position2.position.x;
  fly->position.y = (sinf(angle) * 700) + robot->position2.position.y;


  if (robot->has_traction)
  {
    angle += .005;
  }

//  printf("%f, %f, %f\n", fly->position.x, fly->position.y, fly->heading);
}

void takeoff(struct fly *fly);

void buzz(struct robot *robot)
{

  if (1)
  {
    move_fly(robot, &robot->the_fly);
    robot->the_fly.flight_counter--;
  }
  else if (robot->the_fly.flight_counter == 0)
  {
    robot->the_fly.flight_counter = -((rand() % 1000) + 400);
  }
  else
  {
    robot->the_fly.flight_counter++;

//    fprintf(stderr, "%f\n", robot->range);

    if (robot->the_fly.flight_counter == 0)
    {
      robot->the_fly.flight_counter = (rand() % 2000) + 1000;
    }
  }

//  printf("%d\n", robot->the_fly.flight_counter);

  robot->turret.target = robot->the_fly.position;

  robot->look_angle = atan2f(robot->turret.target.y - robot->position2.position.y,
    robot->turret.target.x - robot->position2.position.x);

//  fprintf(stderr, "%f\n", robot->look_angle);

//  robot->turret.target.x += 1500;
}

