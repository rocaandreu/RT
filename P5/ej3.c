#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int msg_count = 0;

void int_handler(int SIG_NUM)
{
    printf("Total de mensajes imprimidos: %d\n", msg_count);
    exit(0); 
}

int main(int argc, char const *argv[])
{
    struct sigaction hand;
    sigemptyset(&hand.sa_mask);
    hand.sa_flags = 0;
    hand.sa_handler = int_handler;
    
    //Ejecutar int_handler cuando ese reciva SIGINT
    sigaction(SIGINT, &hand, NULL); 
    
    //Recibiendo SIGINT
    while(1) 
    {
        write(1, "Estoy trabajando\n", 18);
        msg_count++;
    }
    return 0;
}