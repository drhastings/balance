#ifndef CONTROL_PANEL__H
#define CONTROL_PANEL__H

#include <stdio.h>
#include <string.h>

#include "position.h"
#include "pd.h"
#include "motor.h"
#include "robot.h"
#include "server.h"
#include "node.h"
#include "page.h"
#include "text.h"
#include "variable.h"
#include "strings.h"

void start_control_panel(struct robot *robot);

#endif
