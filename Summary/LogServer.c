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
#include <pthread.h>
#include "Log.h"

#define SERVER_IP "127.0.0.1"

void *handler(void *client_sockfd)
{
    char buf[MAX_MSG_LENGTH];
    struct msg msg;

    while (1)
    {
        //Leemos longitud del mensaje
        read(client_sockfd, &buf, sizeof(int)); 
        msg.message_length = atoi(&buf);

        //Leemos tipo del mensaje 
        read(client_sockfd, &buf, sizeof(int));
        msg.message_type = atoi(&buf);

        //Leemos lo que queda del mensaje en función del tipo
        if (msg.message_type == START_MSG)
        {
            read(client_sockfd, buf, msg.message_length - 2*sizeof(int)); 
            msg.PID = atoi(&buf);
        }
        else if (msg.message_type == DATA_MSG)
        {
            read(client_sockfd, &buf, sizeof(int)); //Leemos data_lenght
            msg.data_length = atoi(&buf);

            read(client_sockfd, &buf, msg.data_length); //Leemos data
            strcpy(&msg.data, &buf);
        } 
        else if (msg.message_type == UNF_DATA_MSG)
        {
            
        }
        else if (msg.message_type == END_MSG)
        {
            
        }
    }
    pthread_exit(NULL);
}
int main(int argc, char const *argv[])
{
    int logfd;
    char *logfile_name;
    int port = atoi(argv[2]);
    pthread_t th;

    //Abrimos logfile si existe y lo borramos, si no existe lo creamos nuevo
    strcpy(logfile_name, argv[1]);
    if (access(logfile_name, F_OK) == 0)    
    {
        logfd = open(logfile_name, O_RDWR);
        while (read(logfd, NULL, 128) < 128); //vamos leyendo en bloques de 128, hasta EOF jiji
    }
    else
    {
        logfd = open(logfile_name, O_WRONLY|O_CREAT);
        if (logfd < 0) perror("Error creating file\n");
    }

    //Creamos el socket
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //Creamos un adress para asignar posteriormente
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    //Asignamos el adress al socket
    int bind_ret = bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_ret < 0) return EXIT_FAILURE;

    //esperamos a que se establezca conexión
    listen(server_sockfd, 200); //preguntar
    //se establece la conexión, al poner NULL en el segundo parámetro, no especificamos un adress específico
    //al que conectarnos, nos conectaremos a todos los que soliciten conexión
    while (1)
    {
        int client_sockfd = accept(server_sockfd, NULL, NULL);
        pthread_create(&th, NULL, handler, (void *)client_sockfd);//Aun no se lo que tengo que pasarle al handler, lo dejo en NULL pero OJO
    }
    


    close(logfd);
    return 0;
}
