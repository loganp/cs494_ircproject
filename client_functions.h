#ifndef CLI_FUNCT_H
#define CLI_FUNCT_H


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pwd.h>


/* Read stdin and trim trailing newline */
void getstdin (char * buffer);

/* Get hostname and username of user */
void getHostInfo ( char * hostNameBuffer, char * usernameBuffer, int buffersize);

/* Get prompt and get nickname from client */
void getNick (char * buffer, int bufSize);

void createPacket (char * buffer, int size);



#endif
