#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "list.h"

struct waypoint
{
  struct vect point;

  struct list_head list_entry;
};

#endif 
