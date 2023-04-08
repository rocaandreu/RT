#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include "AsciiToInteger.h"


int main(int argc, char *argv[])
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    int time = AsciiToInteger(argv[1]);
    if (argc != 2 || time < 0) return EXIT_FAILURE;
    usleep(time);

    gettimeofday(&end, NULL);

    printf("Real wait time: %d usec\n", end.tv_usec-start.tv_usec);
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