
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


extern int MAX_ROOMS;

extern int SIZE;


/*
  A connected client is encapsulated by this struct
*/
struct clientData {
  char hostname[32],
       username[32],
       nickname[9],
       ip[16];
};



/*
  A room aka channel is only the title and array ofpointers to members
*/
struct room {
  char name[16];
  struct clientData * members; // 8 Max per room
};



/*
  The server encapsulates important info in this struct:
  the array of connected clients and array of exisiting channels
*/
struct serverData {
  struct clientData clients;
  struct room rooms[16];
};



/*
  After a client sends a packet it is the fields are extracted here
*/
struct prsdCommand {
  char command[5],
       params[16],
       message[64];
};




size_t strlcpy (char *, const char *, size_t);



/*
  Extracts client message into prsdCommand struct
  Sends welcome message if new client
  else processes command
*/ // DEPRECATED
int parseMessage (char * message, int bufLen);




int sendMessage (char * buffer, const int socket);


/*
  Copy command list to buffer to be written so socket
*/
void getCommands (char * buffer);



/*
  Matches command to int index for command processing
*/
int commandCheck (char * message);



/*
  Extracts client message into prsdCommand struct
  Sends welcome message if new client
  else processes command
*/
void processMessage (char * mesage, struct serverData * data, const int socket);



/*
  Process command received from client
*/
void doCommand (struct prsdCommand *, struct clientData *, struct serverData *,
                  const int socket); 




void clientQuit (char * mesg, struct clientData * originCli, struct serverData * serverInfo, 
                  const int socket);

#endif
