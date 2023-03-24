#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>


int main() 
{
    //Creamos la región compartida
    int r = shm_open("Test", O_RDWR, 0777);
    int *region = (int*) mmap(NULL, 4096, PROT_READ|PROT_WRITE,MAP_SHARED, r, 0);
    close(r);    

    //Creamos los punteros num, valid, ready que apuntan a una posición de la región
    int *ready = &region[0];
    int *valid = &region[1];
    int *number = &region[2];
    int *length = &region[3];



    for (int i = 0; i < *length; i++)
    {
        while (*valid == 0);   
        printf("%d\n", *number);
        *ready = 1;
        *valid = 0;     

    }

    printf("%d\n", *ready);

    //Unmapeamos memoria y desvinculamos la región
    munmap(region, 4096);
    shm_unlink("Test");
}