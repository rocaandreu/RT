#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

int main(int argc, char const *argv[])
{
    //Creamos un socket y una dirección
    struct sockaddr_in server_addr;
     //Asignamos al servidor la IP del ordenador que corre el programa
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    //Asignamos el socket a una dirección ip y un puerto
    int bind_addr = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    

    return 0;
}
