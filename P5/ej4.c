#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include "AsciiToInteger.h"

int msg_count = 0;

int AsciiToInteger(char *sr1){
   int i, res = 0, digit = 1, length = strlen(sr1);
   for (i = length-1; i >= 0; i--)
   {
      if (sr1[i] < '0' || sr1[i] > '9')
         return -1;

      res += (sr1[i]-'0')*digit;
      digit *= 10;
   }
   return res;
}

void int_handler(int SIG_NUM, siginfo_t *info, void *context)
{
    printf("El proceso %d ha terminado\n", info->si_pid); 
}


int main(int argc, char *argv[])
{
    //Handler
    struct sigaction hand;
    sigemptyset(&hand.sa_mask);
    hand.sa_flags = SA_SIGINFO; //Configuramos la señal para que nos de información de quién la ha mandado
    hand.sa_sigaction = int_handler; //No podemos usar sa_sighandler porque requerimos más detalles que sólo el tipo de señal    sigaction(SIGUSR1, &hand, NULL);
    sigaction(SIGUSR1, &hand, NULL);


    //Creación hijos
    int SonPID;
    int n_process = AsciiToInteger(argv[1]);
    if (n_process <= 0) return EXIT_FAILURE;

    for (int i = 0; i < n_process; i++)
    {
        SonPID = fork();
        if (SonPID == 0) break;
    }

    //Procesos
    if (SonPID == 0)
    {
        //Mandamos SIGUSR1 al proceso padre
        kill(getppid(), SIGUSR1);
        exit(0);
    }
    else
    {
        //Esperamos a que algun proceso acabe
        wait(NULL);    
        return 0;
    }            
}


