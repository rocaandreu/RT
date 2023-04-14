#ifndef LOG_H
#define LOG_H

#define N_CLIENTS 10
#define SERVER_IP "127.0.0.1"
#define MAX_MSG_LENGTH 4096-3*sizeof(int)
#define MAX_DATA_LENGTH 4096-3*sizeof(int)

#define CON_MSG_T 0
#define F_MSG_T 1
#define UNF_MSG_T 2
#define END_MSG_T 3

int send_msg(int fd, struct msg_data msg);

struct msg_con
{
    int message_length; // Longitud total del mensaje
    int message_type; // Tipo de mensaje. En este caso es igual a 0
    int PID; // PID del cliente 
};

struct msg_data
{   
    int message_length; // Longitud total del mensaje
    int message_type;   // Tipo de mensaje. (2 si cabe en uno solo, 1 si necesitaremos varios)
                        // Puede ser 1 o 2 (ver más adelante)
    int data_length;    // Longitud de los datos del mensaje
    char data[4096-3*sizeof(int)]; // Datos del mensaje
};

struct msg_end
{
    int message_length; // Longitud total del mensaje
    int message_type; // Tipo de mensaje. En este caso valdrá 3
};

struct msg
{
    int message_length; // Longitud total del mensaje
    int message_type;   // Tipo de mensaje. (2 si cabe en uno solo, 1 si necesitaremos varios)
                        // Puede ser 1 o 2 (ver más adelante)

    int PID; // PID del cliente 
    int data_length;    // Longitud de los datos del mensaje
    char data[4096-3*sizeof(int)]; // Datos del mensaje
};




#endif
