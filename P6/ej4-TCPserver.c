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
    int newfd;
    char server_message[128];

    //Creamos socket para este proceso (servidor)
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("server_sockfd = %d\n", server_sockfd);

    //Escogemos una dirección IP al socket del servidor
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    //Asignamos el socket que hemos creado al address y el puerto de la máquina que hemos especificado en server_addr
    int bind_ret = bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("bind_ret = %d\n\n", bind_ret);
    if (bind_ret < 0) return EXIT_FAILURE;

    //Escuchamos peticiones de conexión y las aceptamos todas (nos da igual el address de donde vengan por eso los campos son NULL)
    listen(server_sockfd, 5);
    int client_sockfd = accept(server_sockfd, NULL, NULL);

    //Enviamos los números del 1 al 10 y un mensaje;
    int write_ret;
    for (int i = 0; i < 10; i++)
    {
        sprintf(server_message, "Número enviado = %d\n", i);
        //write_ret = send(client_sockfd, &server_message, sizeof(server_message), 0);
        write_ret = write(client_sockfd, &server_message, sizeof(server_message));
        printf("%swrite_ret = %d\n", server_message, write_ret);
    }
    sprintf(server_message, "Ya hemos enviado todos los números :D\n");
    write_ret = write(client_sockfd, &server_message, sizeof(server_message));
    printf("%swrite_ret = %d\n", server_message, write_ret);

    //Cerramos socket
    close(server_sockfd);
    return 0;
}
