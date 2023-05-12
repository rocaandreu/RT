#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "sensors.h"

int init_tasks();
void task_read_pos(void *param);
void task_update_speed_accel(void *param); //Reading period is 300ms
void task_approach_func(void *param);
void task_measure_speed(void *param);



#endif
