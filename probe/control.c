#include "os.h"

#include "control.h"
#include "sensors.h"
#include "actuators.h"

#include <sys/time.h>
#include <math.h>
#include <time.h>

#define SENSOR_DELAY 0.3
#define ACTUATOR_DELAY 50000
#define TOP_SPEED 

//Architecture extern variables
extern PREADYQUEUE tasks_queue1, tasks_queue2;
extern PSCHEDULER sensors_scheduler, main_scheduler;

//Sensors extern variables
extern int sensors_fd[3];

//Actuators extern variables
extern int actuators_fd[6];
int actuator_state[6] = {0, 0, 0, 0, 0, 0};

float pos[4] = {0.0, 0.0, 0.0, 0.0};
float luke_speed[4];
static float temp_p, temp_s;

int end_point[3];
void task_find_endpoint(void *param)
{
    end_point[0] = pos[0];
    end_point[1] = pos[1];
    end_point[2] = pos[2];
    pos[3] = sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);

    end_point[0] += (end_point[0]/pos[3])*4;
    end_point[1] += (end_point[1]/pos[3])*4;
    end_point[2] += (end_point[2]/pos[3])*4;

    printf("ENDPOINT: X = %d, Y = %d, Z = %d\n", end_point[0], end_point[1], end_point[2]);
    return;
}

int check_half[3] = {0, 0, 0};
void task_travel_XZ(void *param) //Reading period is 300ms
{    
    PTASK Vertical_travel;

    Task_create(&Vertical_travel, "Travel in the Y direction", task_travel_Y, NULL, 0, 200);
    Readyqueue_enqueue(tasks_queue2, Vertical_travel);

    //TASK TRAVEL
    float time_x = 0, time_z = 0;

    //X axis (First turn EAST on, then break with WEST)
    time_x = sqrt(fabs((end_point[0]))/0.2);
    activate_actuators(actuators_fd[0], "start");
    int parteEntera = (int) time_x;
    sleep(parteEntera);
    usleep((time_x - parteEntera)*1000000);
    activate_actuators(actuators_fd[0], "stop");
    activate_actuators(actuators_fd[1], "start");
    sleep(parteEntera);
    usleep((time_x - parteEntera)*1000000);
    activate_actuators(actuators_fd[1], "stop");

    //Z axis (First turn UP on, then break with DOWN)
    //time_z = sqrt(fabs((end_point[2])*2)/0.2)/2;
    time_z = sqrt(fabs((end_point[2]))/0.2);
    activate_actuators(actuators_fd[5], "start");
    usleep(time_z*1000000);
    activate_actuators(actuators_fd[5], "stop");
    activate_actuators(actuators_fd[4], "start");
    usleep(time_z*1000000);
    activate_actuators(actuators_fd[4], "stop");

    return;
}


void task_travel_Y(void *param) //Reading period is 300ms
{    
    //TASK TRAVEL
    float time_y = 0;

    //Y axis (First turn SOUTH on, then break with NORTH)
    time_y = sqrt(fabs((end_point[1]))/0.2);
    activate_actuators(actuators_fd[2], "start");
    int parteEntera = (int) time_y;
    sleep(parteEntera);
    usleep((time_y - parteEntera)*1000000);
    activate_actuators(actuators_fd[2], "stop");
    activate_actuators(actuators_fd[3], "start");
    sleep(parteEntera);
    usleep((time_y - parteEntera)*1000000);
    activate_actuators(actuators_fd[3], "stop");

    return;
}

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


    //CREAMOS TASK RECURRENTE
    PTASK task_update = NULL;

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

    time_us[0] = fabs(1000000*(luke_speed[0]/0.2));  //X
    time_us[1] = fabs(1000000*(luke_speed[1]/0.2));  //Y
    time_us[2] = fabs(1000000*(luke_speed[2]/0.2));  //Z

    //Z Axis (UP PUMP)
    printf("ENCENDEMOS Z\n");
    activate_actuators(actuators_fd[5], "start");
    usleep(time_us[2]-ACTUATOR_DELAY);
    activate_actuators(actuators_fd[5], "stop");

    //Y Axis (SOUTH PUMP)
    printf("ENCENDEMOS Y\n");
    activate_actuators(actuators_fd[2], "start");
    usleep(time_us[1]-ACTUATOR_DELAY);
    activate_actuators(actuators_fd[2], "stop");

    //X Axis (WEST PUMP)
    printf("ENCENDEMOS X\n");
    activate_actuators(actuators_fd[1], "start");
    usleep(time_us[0]-ACTUATOR_DELAY);
    activate_actuators(actuators_fd[1], "stop");

    printf("Na naive eishpasial ha sigudo frenaida eishitosameinte / A sonda atingiu a velocidade relativa desejada\n");
    return;
}


int init_tasks()
{
    int main();

    //SCHEDULER 1
    PTASK get_init_speed = NULL, set_init_speed = NULL, task_update = NULL, task_endpoint = NULL;

    Task_create(&get_init_speed, "Get initial speed", task_find_initial_luke_speed, NULL, 0, 100);
    Readyqueue_enqueue(tasks_queue1, get_init_speed);

    Task_create(&set_init_speed, "Set probe to luke speed", task_set_to_luke_speed, NULL, 0, 200);
    Readyqueue_enqueue(tasks_queue1, set_init_speed);

    Task_create(&task_endpoint, "Get endpoint coordinates", task_find_endpoint, NULL, 0, 260);
    Readyqueue_enqueue(tasks_queue1, task_endpoint);

    Task_create(&task_update, "Travel", task_travel_XZ, NULL, 0, 300);
    Readyqueue_enqueue(tasks_queue1, task_update);

    //SCHEDULER 2
    return 0;
}