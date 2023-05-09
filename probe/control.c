#include "os.h"

#include "control.h"
#include "sensors.h"
#include "actuators.h"

#include <sys/time.h>
#include <math.h>

#define SENSOR_DELAY 0.3
#define TOP_SPEED 

//Architecture extern variables
extern PREADYQUEUE tasks_queue;
extern PSCHEDULER sensors_scheduler;

//Sensors extern variables
extern int sensors_fd[3];

//Actuators extern variables
extern int actuators_fd[6];
int actuator_state[6] = {0, 0, 0, 0, 0, 0};

static float speed[4] = {0.0, 0.0, 0.0, 0.0}; //speed[4] is the module
static float accel[4] = {0.0, 0.0, 0.0, 0.0};
static float pos[4] = {0.0, 0.0, 0.0, 0.0};
static float temp_p, temp_s;

void task_read_pos(void *param)
{
    read_sock_active_float(sensors_fd[0], &pos[0]); //X axis
    read_sock_active_float(sensors_fd[1], &pos[1]); //Y axis
    read_sock_active_float(sensors_fd[2], &pos[2]); //Z axis
    pos[3] = sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]); //Module

    return;
}

int end_point[3];
void task_find_endpoint(void *param)
{
    end_point[0] = pos[0];
    end_point[1] = pos[1];
    end_point[2] = pos[2];

    end_point[0] += (end_point[0]/pos[3])*4;
    end_point[1] += (end_point[1]/pos[3])*4;
    end_point[2] += (end_point[2]/pos[3])*4;

    printf("ENDPOINT: X = %d, Y = %d, Z = %d\n", end_point[0], end_point[1], end_point[2]);
    return;
}

int check_half[3] = {0, 0, 0};
void task_update_speed_accel(void *param) //Reading period is 300ms
{    
    read_sock_active_float(sensors_fd[0], &temp_p); //X axis
    temp_s = speed[0];
    speed[0] = (temp_p-pos[0])/SENSOR_DELAY;
    accel[0] = (speed[0]-temp_s)/SENSOR_DELAY;
    pos[0] = temp_p;

    read_sock_active_float(sensors_fd[1], &temp_p); //Y axis
    temp_s = speed[1];
    speed[1] = (temp_p-pos[1])/SENSOR_DELAY;
    accel[1] = (speed[1]-temp_s)/SENSOR_DELAY;
    pos[1] = temp_p;

    read_sock_active_float(sensors_fd[2], &temp_p); //Z axis
    temp_s = speed[2];
    speed[2] = (temp_p-pos[2])/SENSOR_DELAY;
    accel[2] = (speed[2]-temp_s)/SENSOR_DELAY;
    pos[2] = temp_p;

    pos[3] = sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]); //Module

    //TASK TRAVEL
    for (int i = 0; i < 3; i++)
    {
        if (fabs(pos[i]) <= fabs(end_point[i]/2) && check_half[i] == 0)
        {
            if (actuator_state[2*i] == 1)
            {
                activate_actuators(actuators_fd[2*i], "stop");
                activate_actuators(actuators_fd[2*i + 1], "start");
                actuator_state[2*i] = 0;
                actuator_state[2*i + 1] = 1;
            }
            else if (actuator_state[2*i + 1] == 1)
            {
                activate_actuators(actuators_fd[2*i], "start");
                activate_actuators(actuators_fd[2*i + 1], "stop");
                actuator_state[2*i] = 1;
                actuator_state[2*i + 1] = 0;
            }
            check_half[i] = 1;
        }
    }
    return;
}

void task_init_travel(void *param)
{
    //Initial case
    for (int i = 0; i < 3; i++)
    {
        if (pos[i] < 0)
        {
            activate_actuators(actuators_fd[2*i], "start");
            actuator_state[2*i] = 1;  //Empieza en 0
        }
        else if (pos[i] > 0)
        {   
            activate_actuators(actuators_fd[2*i + 1], "start");
            actuator_state[1 + 2*i] = 1;  //Empieza en 1
        }  
    }      
    return;
}


int init_tasks()
{
    int main();
    
    PTASK task_pos_init = NULL, task_update = NULL, task_start = NULL, task_endpoint = NULL;

    Task_create(&task_pos_init, "Get initial position", task_read_pos, NULL, 0, 100);
    Readyqueue_enqueue(tasks_queue, task_pos_init);

    Task_create(&task_endpoint, "Get endpoint coordinates", task_find_endpoint, NULL, 0, 100);
    Readyqueue_enqueue(tasks_queue, task_endpoint);

    //TRAVEL
    Task_create(&task_start, "Start travel", task_init_travel, NULL, 0, 100);
    Readyqueue_enqueue(tasks_queue, task_start);

    //POSITION UPDATE
    Task_create(&task_update, "Update pos, speed, and accel", task_update_speed_accel, NULL, 200, 100);
    Readyqueue_enqueue(tasks_queue, task_update);

    return 0;
}