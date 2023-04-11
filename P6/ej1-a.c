#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char const *argv[])
{
    //Creamos un pipe con nombre "com_pipe" y con permisos de escritura y lectura para el propietario
    mkfifo("com_pipe", 0666); 

    //Abrimos el pipe creado anteriormente en nuestro programa (asignándole un FD) y le damos permiso de escritura y lectura
    int pipefd = open("com_pipe", O_WRONLY);

    //Enviamos los números
    for (int i = 0; i < 10; i++)
    {
        write(pipefd, &i, sizeof(int));
    }
    
    //Cerramos el archivo y lo desvinculamos del programa
    close(pipefd);  
    unlink("com_pipe");
    return 0;
}