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
    char *data_buf = malloc(sizeof(char)*MAX_DATA_LENGTH); //Buffer dinámico para los datos
    char rd_buf[MAX_DATA_LENGTH];
    char PID_buf[11]; //Enough to write "PID 32768: " (largest possible PID number)

    while (1)
    {
        //Leemos longitud del mensaje
        read(client_sockfd, rd_buf, sizeof(int)); 
        msg.message_length = atoi(rd_buf);

        //Leemos tipo del mensaje 
        read(client_sockfd, rd_buf, sizeof(int));
        msg.message_type = atoi(rd_buf);

        //Leemos lo que queda del mensaje en función del tipo
        if (msg.message_type == START_MSG)
        {
            read(client_sockfd, rd_buf, msg.message_length - 2*sizeof(int)); 
            msg.PID = atoi(rd_buf);
            sprintf(PID_buf, "PID %d: ", msg.PID);
            
            pthread_mutex_lock(&logfile_mutex);
            write(logfd, PID_buf, strlen(PID_buf));
            write(logfd, "Connection Started\n", 20);
            pthread_mutex_unlock(&logfile_mutex);
        }
        else if (msg.message_type == DATA_MSG)
        {
            //Leemos data_lenght
            read(client_sockfd, rd_buf, sizeof(int)); 
            msg.data_length = atoi(rd_buf);

            //Leemos data y la añadimos al buffer
            read(client_sockfd, rd_buf, msg.data_length); 
            strcpy(msg.data, rd_buf);
            strcat(data_buf, rd_buf);

            //Escribimos data_buf al logfile (databuf incluye este mensaje y todos los tipo FULL anteriores)
            pthread_mutex_lock(&logfile_mutex);
            write(logfd, PID_buf, strlen(PID_buf));
            write(logfd, data_buf, sizeof(*data_buf));
            pthread_mutex_unlock(&logfile_mutex);

            //Vaciamos data_buf
            i = 1;
            strcpy(data_buf, "");
            data_buf = (char*) realloc(data_buf, MAX_DATA_LENGTH*i);
        } 
        else if (msg.message_type == FULL_DATA_MSG)
        {
            //Leemos data_lenght
            read(client_sockfd, rd_buf, sizeof(int)); 
            msg.data_length = atoi(rd_buf);

            //Leemos data
            read(client_sockfd, rd_buf, msg.data_length);
            strcpy(msg.data, rd_buf);

            //Añadimos nueva data al buffer
            i++;
            strcat(data_buf, rd_buf);
            data_buf = (char*) realloc(data_buf, MAX_DATA_LENGTH*i);
        }
        else if (msg.message_type == END_MSG)
        {
            close(client_sockfd);
            pthread_exit(NULL);
        }
    }
    pthread_exit(NULL);
}


int main(int argc, char const *argv[])
{
    char *logfile_name;
    int port = atoi(argv[2]);
    pthread_t th;

    //Abrimos logfile si existe y lo borramos, si no existe lo creamos nuevo
    strcpy(logfile_name, argv[1]);
    if (access(logfile_name, F_OK) == 0)    
    {
        logfd = open(logfile_name, O_RDWR);
        while (read(logfd, NULL, 128) < 0); //vamos leyendo en bloques de 128, hasta EOF jiji
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

    //esperamos a que se establezca conexión
    listen(server_sockfd, 5); //preguntar
    //se establece la conexión, al poner NULL en el segundo parámetro, no especificamos un adress específico
    //al que conectarnos, nos conectaremos a todos los que soliciten conexión

    int client_sockfd;
    while (1)
    {
        printf("accept working\n");
        client_sockfd = accept(server_sockfd, NULL, NULL);
        pthread_create(&th, NULL, handler, (void *) &client_sockfd);//Aun no se lo que tengo que pasarle al handler, lo dejo en NULL pero OJO
    }
    
    close(logfd);
    return 0;
}