#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char const *argv[])
{
    //Creamos named pipe
    mkfifo("com_pipe", 0666); 
    int pipefd = open("com_pipe", O_WRONLY);

    //Enviamos los números y mensaje al final
    for (int i = 0; i < 10; i++)
    {
        write(pipefd, &i, sizeof(int));
    }

    if(argc == 2) write(pipefd, argv[1], strlen(argv[1]));
    
    //Cerramos el archivo y lo desvinculamos del programa
    close(pipefd);  
    unlink("com_pipe");
    return 0;
}
