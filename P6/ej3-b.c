#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

int main(int argc, char const *argv[])
{
    //Creamos cola y buffer
    mqd_t cola = mq_open("/mq_queue", O_RDONLY);
    char buffer[100];
    int rec_return;

    //Obtenemos atributos de la cola para saber el número de mensajes que contiene
    struct mq_attr attr;
    mq_getattr(cola, &attr);

    //Recibimos mensajes y los mostramos hasta que la cola quede vacía
    while (attr.mq_curmsgs > 0)
    {
        rec_return = mq_receive(cola, buffer, sizeof(buffer), NULL);
        printf(buffer);

        //Sleep y actualizamos los atributos
        sleep(1);
        mq_getattr(cola, &attr);
    }    

    mq_close(cola);
    mq_unlink("/mq_queue");
    return 0;
}