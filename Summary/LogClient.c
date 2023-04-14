#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include "Log.h"

#define SERVER_IP "127.0.0.1"

int main(int argc, char const *argv[])
{
    //Creamos socket para este proceso (cliente)
    int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //Creamos el objeto de la address y puerto del servidor al que nos vamos a conectar (port = 1er param)
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    //Conectamos con el servidor
    int connect_ret = connect(client_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("connect_ret = %d\n", connect_ret);
    
    
    //MAIN LOOP
    int i;
    char *msg_buf;
    char read_buf[1];
    read(1, &read_buf, sizeof(read_buf));
    while (1)
    {
        i = 0;
        while (read_buf != '\n')
        {
            msg_buf[i] = read_buf;
            i++;
        }
    }
    
   
    

    close(client_sockfd);
    return 0;
}