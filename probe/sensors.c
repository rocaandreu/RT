#include "sensors.h"

int init_sensors()
{
    
    return 0;
}


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
