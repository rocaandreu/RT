#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (){
    int SonPID;
    SonPID = fork();
    if (SonPID == 0){
        printf("Soy el hijo (con pid %d), y mi padre es %d\n", getpid(), getppid());
    } 
    else{
        printf("Soy el padre (con pid %d), y mi hijo es %d\n", getpid(), SonPID);
    } 
    return 0;
}