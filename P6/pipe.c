#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char const *argv[])
{
    //Creamos el pipe
    int pipefd[2];
    pipe(pipefd);

    //Proceso hijo y padre
    if (fork() == 0)
    {
        //Cerramos el pipe de escritura
        close(pipefd[WRITE_END]);

        //Imprimir los números
        int num; 
        while (read(pipefd[READ_END], &num, sizeof(int)) > 0)
        {
            printf("Número recibido: %d\n", num);
        }

        //Cerramos pipe y terminamos proceso
        close(pipefd[READ_END]);
    }
    else
    {
        //Cerramos el pipe de lectura
        close(pipefd[READ_END]);

        //Escribimos los números
        for (int i = 0; i < 10; i++)
        {
            write(pipefd[WRITE_END], &i, sizeof(int));
        }

        //Cerramos pipe y terminamos proceso
        close(pipefd[WRITE_END]);
    }

    return 0;
}
