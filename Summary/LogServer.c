#define _XOPEN_SOURCE 600
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

int logfd;
pthread_mutex_t logfile_mutex;

void *handler(void *fd)
{
    int i = 1;
    int client_sockfd = *(int *)fd;
    struct msg msg;
    char data_buf[MAX_DATA_LENGTH*100]; //Hemos intentado usar memoria dinámica para el buffer pero no lo hemos conseguido, son las 1:22AM y queremos dormir :(

    char rd_buf[MAX_MSG_LENGTH];
    char PID_buf[11]; //Enough to write "PID 32768: " (largest possible PID number)

    while (1)
    {
        memset(rd_buf, 0, sizeof(rd_buf));//borramos el buffer

        //Leemos longitud del mensaje
        read(client_sockfd, rd_buf, 4);
        msg.message_length = atoi(rd_buf);

        //Leemos tipo del mensaje 
        read(client_sockfd, rd_buf, 4);
        msg.message_type = atoi(rd_buf);
        memset(rd_buf, 0, sizeof(rd_buf));

        //Leemos lo que queda del mensaje en función del tipo
        if (msg.message_type == START_MSG)
        {
            read(client_sockfd, rd_buf, msg.message_length - 8); 
            msg.PID = atoi(rd_buf);
            sprintf(PID_buf, "PID %d: ", msg.PID);
            printf("cliente: %d\n", client_sockfd);

            pthread_mutex_lock(&logfile_mutex);
            write(logfd, PID_buf, strlen(PID_buf));
            write(logfd, "Connection Started\n", 20);
            pthread_mutex_unlock(&logfile_mutex);
        }
        else if (msg.message_type == DATA_MSG)
        {
            //Leemos data_lenght
            read(client_sockfd, rd_buf, 4); 
            msg.data_length = atoi(rd_buf);
            memset(rd_buf, 0, sizeof(rd_buf));

            //Leemos data y la añadimos al buffer
            read(client_sockfd, rd_buf, msg.data_length); 
            strcpy(msg.data, rd_buf);
            strcat(data_buf, rd_buf);

            //Escribimos data_buf al logfile (databuf incluye este mensaje y todos los tipo FULL anteriores)
            pthread_mutex_lock(&logfile_mutex);
            write(logfd, PID_buf, strlen(PID_buf));
            write(logfd, data_buf, strlen(data_buf));
            pthread_mutex_unlock(&logfile_mutex);
            
            memset(data_buf, 0, sizeof(data_buf));
        }
        else if (msg.message_type == FULL_DATA_MSG)
        {
            //Leemos data_lenght
            read(client_sockfd, rd_buf, 4); 
            msg.data_length = atoi(rd_buf);
            memset(rd_buf, 0, sizeof(rd_buf));
            
            //Leemos data
            read(client_sockfd, rd_buf, msg.data_length);
            strcpy(msg.data, rd_buf);
            
            //Añadimos nueva data al buffer
            strcat(data_buf, rd_buf);
        }
        else if (msg.message_type == END_MSG)
        {
            pthread_mutex_lock(&logfile_mutex);
            write(logfd, PID_buf, strlen(PID_buf));
            write(logfd, "Connection Ended\n", 18);
            pthread_mutex_unlock(&logfile_mutex);

            //Liberamos data_buffer y cerramos socket
            close(client_sockfd);
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}


int main(int argc, char const *argv[])
{
    char logfile_name[strlen(argv[1])];
    int port = atoi(argv[2]);
    pthread_t th;
    
    //Abrimos logfile si existe y lo borramos, si no existe lo creamos nuevo
    strcpy(logfile_name, argv[1]);
    
    if (access(logfile_name, F_OK) == 0)    
    {   
        logfd = open(logfile_name, O_RDWR | O_TRUNC);
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
    printf("bind_ret = %d\n", bind_ret);

    //Esperamos a que se establezca conexión
    listen(server_sockfd, 5); //preguntar
    //Se establece la conexión, al poner NULL en el segundo parámetro, no especificamos un adress específico
    //al que conectarnos, nos conectaremos a todos los que soliciten conexión

    int client_sockfd;
    while (1)
    {
        printf("accept working\n");
        client_sockfd = accept(server_sockfd, NULL, NULL);
        pthread_create(&th, NULL, handler, (void *) &client_sockfd);
    }
    
    close(server_sockfd);
    close(logfd);
    return 0;
}