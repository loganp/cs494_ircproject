
/*
  server_functions.h
*/

#ifndef SERV_FUNCT_H
#define SERV_FUNCT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>






struct clientData {
  char hostname[32],
       username[32],
       nickname[9],
       ip[16];
};

size_t strlcpy (char *, const char *, size_t);


int parseMessage (char * message, int bufLen);

#endif
