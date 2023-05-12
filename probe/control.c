#include "os.h"

#include "control.h"
#include "sensors.h"
#include "actuators.h"

#include <sys/time.h>
#include <math.h>
#include <time.h>

#define SENSOR_DELAY 0.3
#define TOP_SPEED 

//Architecture extern variables
extern PREADYQUEUE tasks_queue1, tasks_queue2;
extern PSCHEDULER sensors_scheduler, main_scheduler;

//Sensors extern variables
extern int sensors_fd[3];

//Actuators extern variables
extern int actuators_fd[6];
int actuator_state[6] = {0, 0, 0, 0, 0, 0};

static float speed[4] = {0.0, 0.0, 0.0, 0.0}; //speed[4] is the module
static float accel[4] = {0.0, 0.0, 0.0, 0.0};
static float pos[4] = {0.0, 0.0, 0.0, 0.0};
static float temp_p, temp_s;

int initial_done = 0;
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



void task_get_luke_speed(void *param)
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












float luke_speed[4];
void task_find_initial_luke_speed(void *param)
{
    float init_pos[3];
    //LEER POS 1a VEZ
    read_sock_active_float(sensors_fd[0], &init_pos[0]); //X axis
    read_sock_active_float(sensors_fd[1], &init_pos[1]); //Y axis
    read_sock_active_float(sensors_fd[2], &init_pos[2]); //Z axis

    //Leemos más veces para tener más exactitud
    for (int i = 0; i < 5; i++)
    {
        read_sock_active_float(sensors_fd[0], &temp_p); //X axis
        read_sock_active_float(sensors_fd[1], &temp_p); //Y axis
        read_sock_active_float(sensors_fd[2], &temp_p); //Z axis
    }
    

    //LEER POS ÚLTIMA VEZ y CALCULAR VELOCIDAD
    read_sock_active_float(sensors_fd[0], &temp_p); //X axis
    luke_speed[0] = ((temp_p - init_pos[0]))/(6*(SENSOR_DELAY));

    read_sock_active_float(sensors_fd[1], &temp_p); //Y axis
    luke_speed[1] = ((temp_p - init_pos[1]))/(6*SENSOR_DELAY);

    read_sock_active_float(sensors_fd[2], &temp_p); //Z axis
    luke_speed[2] = ((temp_p - init_pos[2]))/(6*SENSOR_DELAY);

    printf("LUKE SPEED: X = %f m/s, Y = %f m/s, Z = %f m/s\n", luke_speed[0], luke_speed[1], luke_speed[2]);


    //CREAMOS TASKS RECURRENTE
    PTASK task_pos_init = NULL, task_update = NULL;

    Task_create(&task_pos_init, "Get initial position", task_read_pos, NULL, 0, 100);
    Readyqueue_enqueue(tasks_queue2, task_pos_init);

    Task_create(&task_update, "Get speed", task_measure_speed, NULL, 400, 300);
    Readyqueue_enqueue(tasks_queue2, task_update);


    return;
}


void task_measure_speed(void *param)
{
    read_sock_active_float(sensors_fd[2], &temp_p); //Z axis
    temp_s = luke_speed[2];
    luke_speed[2] = (temp_p-pos[2])/SENSOR_DELAY;
    pos[2] = temp_p;

    read_sock_active_float(sensors_fd[1], &temp_p); //Y axis
    temp_s = luke_speed[1];
    luke_speed[1] = (temp_p-pos[1])/SENSOR_DELAY;
    pos[1] = temp_p;

    read_sock_active_float(sensors_fd[0], &temp_p); //X axis
    temp_s = luke_speed[0];
    luke_speed[0] = (temp_p-pos[0])/SENSOR_DELAY;
    pos[0] = temp_p;

    printf("LUKE SPEED: X = %f m/s, Y = %f m/s, Z = %f m/s\n", luke_speed[0], luke_speed[1], luke_speed[2]);

    return;
}


void task_set_to_luke_speed(void *param)
{
    float time_us[3];
    //struct timeval start, culo;

    time_us[0] = fabs(1000000*(luke_speed[0]/0.2));  //X
    time_us[1] = fabs(1000000*(luke_speed[1]/0.2));  //Y
    time_us[2] = fabs(1000000*(luke_speed[2]/0.2));  //Z

    //Z Axis (WEST PUMP)
    printf("ENCENDEMOS Z\n");
    activate_actuators(actuators_fd[4], "start");
    usleep(time_us[2]);
    activate_actuators(actuators_fd[4], "stop");

    //Y Axis (SOUTH PUMP)
    printf("ENCENDEMOS Y\n");
    activate_actuators(actuators_fd[2], "start");
    usleep(time_us[1]);
    activate_actuators(actuators_fd[2], "stop");

    //X Axis (WEST PUMP)
    printf("ENCENDEMOS X\n");
    activate_actuators(actuators_fd[1], "start");
    usleep(time_us[0]);
    activate_actuators(actuators_fd[1], "stop");

    printf("PUTA\n");
    return;
}



void task_correct_speed(void *param)
{
    sleep(1);
    //X AXIS
    while (luke_speed[0] < -0.015) //WEST
    {
        if (actuator_state[0] == 0)
        {
            printf("ENTRO 1\n");
            activate_actuators(actuators_fd[0], "start");
            actuator_state[0] = 1;
        } 
    }
    if (actuator_state[0] == 1)
    {
        printf("APAGO 1\n");
        activate_actuators(actuators_fd[0], "stop");
        actuator_state[0] = 0;
    }
    while (luke_speed[0] > 0.015) //EAST
    {   
        if (actuator_state[1] == 0)
        {
            printf("ENTRO 2\n");
            activate_actuators(actuators_fd[1], "start");
            actuator_state[1] = 1;
        }
    }
    if (actuator_state[1] == 1)
    {
        printf("APAGO 2\n");
        activate_actuators(actuators_fd[1], "stop");
        actuator_state[1] = 0;
    }

    

    return;
}


int init_tasks()
{
    int main();
    
    /*PTASK task_pos_init = NULL, task_update = NULL, task_start = NULL, task_endpoint = NULL;

    Task_create(&task_pos_init, "Get initial position", task_read_pos, NULL, 0, 100);
    Readyqueue_enqueue(tasks_queue, task_pos_init);

    Task_create(&task_endpoint, "Get endpoint coordinates", task_find_endpoint, NULL, 0, 100);
    Readyqueue_enqueue(tasks_queue, task_endpoint);

    //TRAVEL
    Task_create(&task_start, "Start travel", task_init_travel, NULL, 0, 100);
    Readyqueue_enqueue(tasks_queue, task_start);

    //POSITION UPDATE
    Task_create(&task_update, "Update pos, speed, and accel", task_update_speed_accel, NULL, 200, 100);
    Readyqueue_enqueue(tasks_queue, task_update);*/

    //SCHEDULER 1

    PTASK get_init_speed = NULL, set_init_speed = NULL, correct_speed = NULL;

    Task_create(&get_init_speed, "Get initial speed", task_find_initial_luke_speed, NULL, 0, 100);
    Readyqueue_enqueue(tasks_queue1, get_init_speed);

    Task_create(&set_init_speed, "Set probe to luke speed", task_set_to_luke_speed, NULL, 0, 200);
    Readyqueue_enqueue(tasks_queue1, set_init_speed);

    Task_create(&correct_speed, "Correct speed", task_correct_speed, NULL, 0, 300);
    Readyqueue_enqueue(tasks_queue1, correct_speed);

    //SCHEDULER 2

    /*PTASK task_pos_init = NULL, task_update = NULL;

    Task_create(&task_pos_init, "Get initial position", task_read_pos, NULL, 0, 100);
    Readyqueue_enqueue(tasks_queue2, task_pos_init);

    Task_create(&task_update, "Update pos, speed, and accel", task_measure_speed, NULL, 400, 300);
    Readyqueue_enqueue(tasks_queue2, task_update);*/

    return 0;
}