#include "../includes/fly.h"
#include "../includes/robot.h"
#include "../includes/coord.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void init_fly(struct fly *fly)
{
  fly->position.x = 1000;
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

void move_fly(struct fly *fly)
{
  int direction = rand() % 100;

  if (direction <= 25)
    fly->heading += .25;

  if (direction >= 75)
    fly->heading -= .25;


  fly->heading = normalized_angle(fly->heading);

  fly->position.x += cosf(fly->heading) * fly->speed;
  fly->position.y += sinf(fly->heading) * fly->speed;

  float distance = sqrtf(fly->position.x * fly->position.x +
        fly->position.y * fly->position.y);
  
  if (distance > 1000)
  {
    fly->position.x = (fly->position.x / distance) * 1000;
    fly->position.y = (fly->position.y / distance) * 1000;
  }

//  printf("%f, %f, %f\n", fly->position.x, fly->position.y, fly->heading);
}

void takeoff(struct fly *fly);

void buzz(struct robot *robot)
{

  if (robot->the_fly.flight_counter > 0)
  {
    move_fly(&robot->the_fly);
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

  printf("%f, %f\n", robot->dest->x, robot->dest->y);

//  robot->turret.target.x += 1500;
}

