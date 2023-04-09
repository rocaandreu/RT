#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include "AsciiToInteger.h"
#include <pthread.h>

const int Nthreads = 7;
void *threadwait(void* tme)
{   
    long long t = (long long)tme;
    struct timeval start1, actual;
    gettimeofday(&start1, NULL);
    while((actual.tv_usec - start1.tv_usec) < t)
    {
        gettimeofday(&actual, NULL);
    }  
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int time = AsciiToInteger(argv[1]);
    long long timelonged = (long long)time;
    if (argc != 2 || time < 0) return EXIT_FAILURE;
    pthread_t thr[Nthreads];
    struct timeval start, end;
    long int diff = 0;
    gettimeofday(&start, NULL);
    
    for (int i = 0; i < Nthreads; i++)
    {
        pthread_create(&thr[i], NULL, threadwait, (void *) timelonged);
    }
    for (int i = 0; i < Nthreads; i++)
    {
        pthread_join(thr[i], NULL);
    }
    gettimeofday(&end, NULL);
    diff = (end.tv_usec-start.tv_usec) - timelonged;
    printf("Real wait time: %ld usec\n", end.tv_usec-start.tv_usec);
    printf("Difference: %ld usec\n", diff);

    
    return 0;
}

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
//En este caso (con espera activa) podemos ver que la diferencia de tiempos se ve afectada notablemente al superar el número de threads físicos.
//Esto se debe a que, a diferencia que con la función sleep, cuando realizamos una espera activa hacemos uso de la CPU
//Hemos visto que para pausar un solo programa es mucho más eficaz realizar una espera activa, ya que en general los recursos que utilices no 
//tendrán una repercusión excesivamente negativa, y a cambio se gana precisión. Por otro lado, cuando se pretendan pausar distintos programas a
//la vez será recomendable utilizar funciones como usleep()