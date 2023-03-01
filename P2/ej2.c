#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

long a = 0;

int main (){
    int SonPID;
    SonPID = fork();
    while (SonPID == 0){
        a++;
        printf("%ld\n", a);
        usleep(50000);
    } 
    while (SonPID != 0){
        a--;
        printf("%ld\n", a);
        usleep(50000);
    } 
    return 0;
}

/*
Se observa en la ejecución que aunque la variable sea global,
cada proceso la tiene en su memoria particular.
*/