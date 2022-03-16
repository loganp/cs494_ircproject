#ifndef CLI_FUNCT_H
#define CLI_FUNCT_H


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pwd.h>

extern int SIZE;

struct userInfo {
  char hostname[32];
  char username[16];
  char nickname[9];
};

/*
  check if string is alphanumeric
*/
int stralnum (char * str);



/* 
Read stdin and trim trailing newline 
*/
void getstdin (char * buffer);

/* Get hostname and username of user */
void getHostInfo (struct userInfo * info);

/* Get prompt and get nickname from client */
void getNick (char * buffer, int bufSize);

int sendMessage (struct userInfo * info, char * buffer, const int socket);

void createPacket (struct userInfo * userinfo, char msg[]);



#endif
