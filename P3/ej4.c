#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

long a[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void *th_function(void *i)
{
    long long k = (long long)i;
    for (int j = 0; j < 10000; j++)
        a[k]++;

    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    //Create threads
    pthread_t th[10];
    for (long i = 0; i < 10; i++)
        pthread_create(&th[i], NULL, th_function, (void *)i);

    //Wait for all threads to finish and then print results
    for (int i = 0; i < 10; i++)
        pthread_join(th[i], NULL);

    for (int i = 0; i < 10; i++)
        printf("%ld\n", a[i]);


    return 0;
}