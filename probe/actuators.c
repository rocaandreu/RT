#include "actuators.h"

static int connect_socket(int port, int *fd)
{
    struct sockaddr_in serv_addr;
    int try;

    if ((*fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    try=0;

    while ((connect(*fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) && (try < 5))
    {
        try++;
        usleep(500000);
    }

    if (try==5) return -1;
    
    return 0;
}

int init_actuators()
{
    int actuators_fd[6];

    if (connect_socket(20000, &actuators_fd[0]) < 0) return -1; //Pump N
    if (connect_socket(20001, &actuators_fd[1]) < 0) return -2; //Pump S
    if (connect_socket(20002, &actuators_fd[2]) < 0) return -3; //Pump E
    if (connect_socket(20003, &actuators_fd[3]) < 0) return -4; //Pump W
    if (connect_socket(20004, &actuators_fd[4]) < 0) return -5; //Pump UP
    if (connect_socket(20005, &actuators_fd[5]) < 0) return -5; //Pump DOWN

    printf("Connection to all actuators was successful: fd_N = %d, fd_S = %d, fd_E = %d, fd_W = %d, fd_UP = %d, fd_DOWN = %d\n", actuators_fd[0], actuators_fd[1], actuators_fd[2], actuators_fd[3], actuators_fd[4], actuators_fd[5]);

    return 0;
}

