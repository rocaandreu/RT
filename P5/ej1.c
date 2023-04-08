#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void int_handler()
{
    write(1, "Programa interrupido\n", 22);
    exit(0);
}

int main(int argc, char const *argv[])
{
    char buffer[128];
    struct sigaction hand;

    //Creamos una mascara (señales que queremos bloquear mientras se ejecute el handler),
    //en este caso no queremos bloquear ninguna, por lo que dejamos el conjunto vacío.
    sigset_t mask;
    sigemptyset(&mask);
    hand.sa_mask = mask;

    //Configuración por defecto
    hand.sa_flags = 0;

    //Indicamos int_handler como handler para interrupciones
    hand.sa_handler = int_handler;
    
    //Ejecutar int_handler cuando ese reciva SIGINT
    sigaction(SIGINT, &hand, NULL); 
    
    //Recibiendo SIGINT
    while(1) 
    {
        sprintf(buffer, "I am working hard\n");
        write(1, buffer, 19);
        sleep(0.1);
    }
    return 0;
}
