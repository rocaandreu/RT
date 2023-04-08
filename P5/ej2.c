#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>

int SIGUSR1_counter = 0;

void int_handler(int SIG_NUM)
{
    if (SIG_NUM == SIGUSR1)
    {
        SIGUSR1_counter++;
    }
    else if (SIG_NUM == SIGUSR2)
    {
        printf("SIGUSR1 recibidos: %d\n", SIGUSR1_counter);
        exit(0);
    }
}

int main(int argc, char const *argv[])
{   
    struct sigaction hand;
    
    //Creamos una máscara vacía
    sigemptyset(&hand.sa_mask);

    //Configuración por defecto
    hand.sa_flags = 0;

    //Indicamos qué handler usamos
    hand.sa_handler = int_handler;
    
    //Ejecutar int_handler cuando ese reciva SIGINT
    sigaction(SIGUSR1, &hand, NULL); 
    sigaction(SIGUSR2, &hand, NULL); 


    int SonPID;
    SonPID = fork();

    if (SonPID == 0) //Proceso hijo
    {
        //Recibiendo SIGUSR1 y SIGUSR2
        while(1);
        return 0;
    }
    else //Proceso principal
    {
        //Enviamos señales
        for (int i = 0; i < 1000; i++) 
            kill(SonPID, SIGUSR1);
        kill(SonPID, SIGUSR2);

        //Esperamos a que terminen todos los procesos hijo
        wait(NULL);

        return 0;
    }
}

//Se pierden la mayoría de señales porque se reciben mientras el proceso hijo está
//ejecutando el handler, lo cual hace que no las pueda procesar correctamente.