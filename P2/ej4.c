#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main (){
    int SonPID, i;
    for (i = 0; i < 10; i++){
        SonPID = fork();
        if (SonPID == 0)
        {
            for (int j = 0; j < 3; j++)
            {
                printf("%d, i = %d\n", getpid(), i);

            }
            break;
        }   
    }
    usleep(10000); //Hay que ponerlo porque sino el terminal se vuelve loco, pero molaria saber como evitarlo xd
    return 0;
}