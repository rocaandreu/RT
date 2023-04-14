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


int main(int argc, char const *argv[])
{
    int logfd;
    char *logfile_name;
    strcpy(logfile_name, argv[1]);
    if (access(logfile_name, F_OK) == 0)    
    {
        logfd = open(logfile_name, O_RDWR);
        while (read(logfd, NULL, 128) < 128); //vamos leyendo en bloques de 128, hasta EOF jiji
    }
    else
    {
        logfd = open(logfile_name, O_RDWR);
        if (logfd < 0) perror("Error creating file\n");
    }


    
    close(logfd);
    return 0;


}