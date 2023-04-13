#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9002

int main(int argc, char const *argv[])
{
    //Creamos socket para este proceso (cliente)
    int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //Creamos el objeto de la address del servidor para poder conectarnos a el
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    //Conectamos con el servidor
    int connect_ret = connect(client_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("connect_ret = %d\n", connect_ret);

    //Leemos todos los mensajes y los mostramos en consola
    char buff[128];
    int read_ret = 1;
    while (read_ret != 0 && read_ret != -1)
    {
        //read_ret = recv(client_sockfd, &buff, sizeof(buff), 0);
        read_ret = read(client_sockfd, buff, sizeof(buff));
        printf("read_ret = %d\n", read_ret);
        printf(buff);
    }

    //Cerramos socket
    close(client_sockfd);
    return 0;
}
