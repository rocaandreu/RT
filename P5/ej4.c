#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include "ej4.h"

int msg_count = 0;

void int_handler(int SIG_NUM, siginfo_t *info, void *context)
{
    printf("El proceso %d ha terminado\n", info->si_pid);
    exit(0); 
}


int main(int argc, const char *argv[])
{
    //Handler
    struct sigaction hand;
    sigemptyset(&hand.sa_mask);
    hand.sa_flags = SA_SIGINFO; //Configuramos la señal para que nos de información de quién la ha mandado
    hand.sa_sigaction = int_handler; //No podemos usar sa_sighandler porque requerimos más detalles que sólo el tipo de señal    sigaction(SIGUSR1, &hand, NULL);


    //Creación hijos
    int SonPID;
    int n_process = AsciiToInteger(argv[1]);
    if (n_process <= 0) return EXIT_FAILURE;

    for (int i = 0; i < n_process; i++)
    {
        SonPID = fork();
    }

    if (SonPID == 0) //Proceso hijo
    {
        //Recibiendo SIGUSR1 y SIGUSR2
        kill(getppid(), SIGUSR1);
        return 0;
    }
    else //Proceso principal
    {
        pause();
   
        //Esperamos a que terminen todos los procesos hijo
        wait(NULL);

        return 0;
    }
}


int AsciiToInteger(char *sr1){
   int i, res = 0, digit = 1, length = strlen(sr1);
   for (i = length-1; i >= 0; i--)
   {
      if (sr1[i] < '0' || sr1[i] > '9')
         return 0;

      res += (sr1[i]-'0')*digit;
      digit *= 10;
   }
   return res;
}