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
#include "Log.h"
#include <signal.h>
#include <unistd.h>

int serverfd = 1;

void int_handler(int SIG_NUM)
{
    struct msg END;    
    END.message_type = END_MSG;
    END.message_length = 2*sizeof(int);
    send_msg(serverfd, END);
    printf("\nDisconnected from server\n");
    exit(0); 
}


int main(int argc, char const *argv[])
{
    //Nuevo handle SIGKILL para enviar mensaje de desconexión
    struct sigaction hand;
    sigemptyset(&hand.sa_mask);
    hand.sa_flags = 0;
    hand.sa_handler = int_handler;
    sigaction(SIGINT, &hand, NULL);

    //Creamos socket para este proceso (cliente)
    int client_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //Creamos el objeto de la address y puerto del servidor al que nos vamos a conectar (port = 1er param)
    if (argc < 2) perror("Error: Missing argument: PORT_NUMBER\n");
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
 
    //Conectamos con el servidor y enviamos mensaje de conexión
    serverfd = connect(client_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("connect_ret = %d\n", serverfd);

    struct msg msg;
    msg.message_type = START_MSG;
    msg.message_length = 3*sizeof(int);
    msg.PID = getpid();
    send_msg(serverfd, msg);
    
    //MAIN LOOP
    char buf;
    while (1)
    {
        buf = '\0';
        msg.message_length = 0;
        msg.data_length = 0;
        msg.message_type = DATA_MSG;
        
        //Leemos hasta Max_data length o hasta salto de línea
        while (msg.data_length < MAX_DATA_LENGTH && buf != '\n')
        {
            read(1, &buf, sizeof(buf));
            msg.data[msg.data_length] = buf;
            msg.data_length++;
        }

        //Definimos el tipo de mensaje en función de si hay salto de línea o no
        if (msg.data_length == MAX_DATA_LENGTH)
        {
            msg.message_type = UNF_DATA_MSG;
            msg.message_length = MAX_MSG_LENGTH;
            send_msg(serverfd, msg);
        }
        else
        {
            msg.message_type = DATA_MSG;
            msg.message_length = msg.data_length + 2*sizeof(int);
            send_msg(serverfd, msg);
        }
    }

    //Cerramos socket
    close(client_sockfd);
    return 0;
}

int send_msg(int fd, struct msg msg)
{
    int res = 0, ret_wr = 0;
    char buf[MAX_MSG_LENGTH];

    //Enviamos mensaje según tipo
    if (msg.message_type == START_MSG)
    {
        //Los enteros deben codificarse con %4d para que cuadre el message_length con la longitud del string que enviamos
        sprintf(buf, "%4d%4d%4d", msg.message_length, msg.message_type, msg.PID);
        ret_wr = write(fd, &buf, msg.message_length);
        return ret_wr;
    }
    else if (msg.message_type == DATA_MSG || msg.message_type == UNF_DATA_MSG)
    {
        sprintf(buf, "%4d%4d%4d%s", msg.message_length, msg.message_type, msg.data_length, msg.data);
        ret_wr = write(fd, &buf, msg.message_length);
        return ret_wr;
    }
    else if (msg.message_type == END_MSG)
    {
        sprintf(buf, "%4d%4d", msg.message_length, msg.message_type);
        ret_wr = write(fd, &buf, msg.message_length);
        return ret_wr;
    }    

    return 0;
}