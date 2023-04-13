#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define server_ip "127.0.0.1"
#define server_port 9002

int main(int argc, char const *argv[])
{
    //Creamos un socket que será el cliente
    int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //Creamos el objeto la address del servidor para poder conectarnos a el
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(9002);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    //Conectamos con el servidor
    int connect_ret = connect(client_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("connect_ret = %d\n", connect_ret);

    //Leemos un mensaje
    char buff[128];
    int read_ret = read(client_sockfd, buff, 45);
    //int read_ret = recv(client_sockfd, &buff, sizeof(buff), 0);
    printf("read_ret = %d\n", read_ret);
    printf(buff);

    //Cerramos socket
    close(client_sockfd);
    return 0;
}
