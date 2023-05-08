#ifndef __SENSORS_H__
#define __SENSORS_H__

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
#include <signal.h>
#include <unistd.h>

int init_sensors(); //0 if success, -1 if error
static int connect_socket(int port, int *fd); //0 if success, -1 if error

#endif
