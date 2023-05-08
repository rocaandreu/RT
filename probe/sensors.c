#include "sensors.h"

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

int init_sensors()
{
    int sensors_fd[3];
    if (connect_socket(20015, &sensors_fd[0]) < 0) return -1; //X axis
    if (connect_socket(20016, &sensors_fd[1]) < 0) return -2; //Y axis
    if (connect_socket(20017, &sensors_fd[2]) < 0) return -3; //Z axis
    printf("Connection to all sensors was successful: fd_X = %d, fd_Y = %d, fd_Z = %d\n", sensors_fd[0], sensors_fd[1], sensors_fd[2]);

    return 0;
}