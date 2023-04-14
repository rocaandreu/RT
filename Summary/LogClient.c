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
    int serverfd = connect(client_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("connect_ret = %d\n", serverfd);
    
    
    //MAIN LOOP
    int i;
    char buf;
    while (1)
    {
        i = 0;
        struct msg msg; 

        //Leemos hasta Max_data length o hasta salto de línea
        while (i < MAX_DATA_LENGTH && buf != '\n')
        {
            read(1, &buf, sizeof(buf));
            msg.data[i] = buf;
            i++;
        }

        //Definimos el tipo de mensaje en función de si hay salto de línea o no
        if (i == MAX_DATA_LENGTH)
        {
            msg.message_type = UNF_MSG_T;
            msg.data_length = MAX_DATA_LENGTH;
            msg.message_length = MAX_MSG_LENGTH;
            send_msg(serverfd, msg);
        }
        else
        {
            msg.message_type = F_MSG_T;
        }
        
        
        //CREAR UN SIGHAND para SIGKILL que envíe un mensage de terminar conexión antes de 
    }
    
    


    close(client_sockfd);
    return 0;
}



int send_msg(int fd, struct msg msg)
{
    int res = 0, ret_wr = 0;
    char buf[MAX_MSG_LENGTH];

    //Enviamos la cabecera del mensaje
    if (msg.message_type == CON_MSG_T)
    {
        //Los enteros deben codificarse con %4d para que cuadre el message_length con la longitud del string que enviamos
        sprintf(&buf, "%4d%4d%4d", msg.message_length, msg.message_type, msg.PID);
        ret_wr = write(fd, &buf, msg.message_length);
        return ret_wr;
    }
    else if (msg.message_type == F_MSG_T || msg.message_type == UNF_MSG_T)
    {
        sprintf(&buf, "%4d%4d%4d%s", msg.message_length, msg.message_type, msg.data_length, msg.data);
        ret_wr = write(fd, &buf, msg.message_length);
        return ret_wr;
    }
    
    

    //Enviamos longitud datos
    ret_wr = write(fd, msg.data_length, sizeof(int));
    if (ret_wr < 0) return -1;
    res += ret_wr;

    //Enviamos datos
    ret_wr = write(fd, msg.data, strlen(msg.data));
    if (ret_wr < 0) return -1;
    res += ret_wr;

    return 0;
}