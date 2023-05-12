#include "os.h"

#include "architecture.h"

#include "simulation_parameters.h"

// Put all the architecture related initialization code here
PREADYQUEUE tasks_queue1 = NULL, tasks_queue2 = NULL;
PSCHEDULER sensors_scheduler = NULL, main_scheduler = NULL;
int init_architecture()
{   
    Readyqueue_create(&tasks_queue1);
    Scheduler_create(&sensors_scheduler, SCHEDULING_SHORTEST_JOB_FIRST, tasks_queue1, 1);

    Readyqueue_create(&tasks_queue2);
    Scheduler_create(&main_scheduler, SCHEDULING_SHORTEST_JOB_FIRST, tasks_queue2, 1);
    return 0;
}
