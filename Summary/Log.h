#ifndef LOG_H
#define LOG_H

#define N_CLIENTS 10
#define SERVER_IP "127.0.0.1"
#define MAX_MSG_LENGTH 4096-3*sizeof(int)
#define MAX_DATA_LENGTH 4096-3*sizeof(int)

#define START_MSG 0
#define DATA_MSG 1
#define FULL_DATA_MSG 2
#define END_MSG 3

struct msg
{
    int message_length; // Longitud total del mensaje
    int message_type;   // Tipo de mensaje. (0 par START, 2 si cabe en uno solo, 1 si necesitaremos varios, 2 para END)

    int PID; // PID del cliente 
    int data_length;    // Longitud de los datos del mensaje
    char data[4096-3*sizeof(int)]; // Datos del mensaje
};

int send_msg(int fd, struct msg msg);

#endif
