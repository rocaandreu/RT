#ifndef LOG_H
#define LOG_H

#define N_CLIENTS 10

struct msg_con
{
    int message_length; // Longitud total del mensaje
    int message_type; // Tipo de mensaje. En este caso es igual a 0
    int PID; // PID del cliente 
};

struct msg_data
{   
    int message_length; // Longitud total del mensaje
    int message_type;   // Tipo de mensaje.
                        // Puede ser 1 o 2 (ver más adelante)
    int data_length;    // Longitud de los datos del mensaje
    char data[4096-3*sizeof(int)]; // Datos del mensaje
};

struct msg_end
{
    int message_length; // Longitud total del mensaje
    int message_type; // Tipo de mensaje. En este caso valdrá 3
};




#endif
