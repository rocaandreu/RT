#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

int main(int argc, char const *argv[])
{
    char msg[50];
    int send_ret;

    //Declaramos atributos de la cola de mensages
    struct mq_attr attributes = {
        .mq_flags = 0,              //Configuration flags
        .mq_maxmsg = 10,            //Maximum number of messages
        .mq_curmsgs = 0,            //Number of messages currently queued
        .mq_msgsize = 60            //Maximum message size
    };

    //Creamos cola (el nombre tiene que empezar con /mq_ para que sea válida en el sist. de archivos)
    mqd_t cola = mq_open("/mq_queue", O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR, &attributes);

    //Enviamos números del 1 al 10
    for (int i = 0; i < 10; i++)
    {
        //Enviamos el número y el retorno del send (0 para success y -1 para error) 
        sprintf(msg, "Número enviado: %d, retorno mq_send: %d\n", i, send_ret);
        send_ret = mq_send(cola, msg, sizeof(msg), 0);
        
        //Sleep para tener tiempo a empezar a ejecutar el receptor (ej3-b.c)
        sleep(1);
    }

    //Enviamos un mensage diferente
    sprintf(msg, "Esto es un mensaje\n");
    send_ret = mq_send(cola, msg, sizeof(msg), 0);

    //Cerramos cola
    mq_close(cola);
    mq_unlink("/mq_queue");
    return 0;
} 