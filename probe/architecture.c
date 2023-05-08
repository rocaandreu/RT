#include "os.h"

#include "architecture.h"

#include "simulation_parameters.h"

// Put all the architecture related initialization code here
PREADYQUEUE tasks_queue = NULL;
PSCHEDULER sensors_scheduler = NULL;
int init_architecture()
{   
    Readyqueue_create(&tasks_queue);
    Scheduler_create(&sensors_scheduler, SCHEDULING_SHORTEST_JOB_FIRST, tasks_queue, 1);
    return 0;
}
