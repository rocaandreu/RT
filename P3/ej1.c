#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *th1_function()
{
    printf("Hola!\n");
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    //Create thread
    pthread_t th1;
    pthread_create(&th1, NULL, th1_function, NULL);

    //Wait for th1 to finish
    pthread_join(th1, NULL);
    printf("Adios!\n");

    return 0;
}