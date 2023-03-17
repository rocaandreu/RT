#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t  mutex;
long a[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void *th1_function()
{
    for (int i = 0; i < 10000; i++)
    {
        pthread_mutex_lock(&mutex);
        for (int j = 0; j < 10; j++)
        {
            a[j]++;
        }
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    //Initialize mutex
    pthread_mutex_init(&mutex, NULL);

    //Create thread
    pthread_t th1;
    pthread_create(&th1, NULL, th1_function, NULL);

    for (int i = 0; i < 10000; i++)
    {
        pthread_mutex_lock(&mutex);
        for (int j = 0; j < 10; j++)
        {
            a[j]++;
        }
        pthread_mutex_unlock(&mutex);
    }     

    //Delete mutex
    pthread_mutex_destroy(&mutex);

    //Wait for th1 to finish and print a
    pthread_join(th1, NULL);
    for (int i = 0; i < 10; i++)
    {
        printf("%ld\n", a[i]);
    }
    return 0;
}