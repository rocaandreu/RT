#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char const *argv[])
{
    //Abrimos el pipe creado en el programa a) y le damos permiso de escritura y lectura
    int pipefd = open("com_pipe", O_RDONLY);

    //Imprimimos todos los números escritos en el pipe
    int num;
    while (read(pipefd, &num, sizeof(int)) > 0)
    {
        printf("Número recibido: %d\n", num);
    }

    //Cerramos el archivo y lo desvinculamos del programa
    close(pipefd);
    unlink("com_pipe");
    return 0;
}