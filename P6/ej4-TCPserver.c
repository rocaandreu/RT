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
    int newfd;
    char server_message[128] = "Te has conectado correctamente al servidor!\n";


    //Asignamos al servidor la IP del ordenador que corre el programa
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("server_sockfd = %d\n", server_sockfd);
    

    //Escogemos una dirección IP al socket del servidor
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(9002);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);


    //Asignamos el socket que hemos creado al puerto de la máquina (le decimos al OS que asigne esa addr y puerto a nuestro socket)
    int bind_ret = bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("bind_ret = %d\n", bind_ret);
    if (bind_ret < 0) return EXIT_FAILURE;


    //Escuchamos peticiones de conexión, las aceptamos todas y escribimos un mensaje
    listen(server_sockfd, 5);
    int client_sockfd = accept(server_sockfd, NULL, NULL);
    //int send_ret = send(client_socketfd, &server_message, sizeof(server_message), 0);
    int write_ret = write(client_sockfd, &server_message, sizeof(server_message));
    printf("write_ret = %d\n", write_ret);

    //Cerramos socket
    close(server_sockfd);
    return 0;
}
