#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

long a = 0;

void *th1_function()
{
    //a--
    while(1){
        a--;
        printf("%ld\n", a);
        usleep(500000);             //Delay to properly observe results
    }  
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    //Create thread
    pthread_t th1;
    pthread_create(&th1, NULL, th1_function, NULL);

    //a++
    while(1){
        a++;
        printf("%ld\n", a);
        usleep(500000);             //Delay to properly observe results
    }     
    printf("Adios!\n");

    return 0;
}

/* El resultado es el esperado, como los hilos comparten memória, aumentan y disminuyen el valor de la misma variable,
por lo que vemos 0 y 1 alternado en la consola*/