/*****
* server.c
*
* simple TCP server on port 7000
******/

#include "server_functions.h"


int main ()
{
  int welcomeSocketfd,
      connectionSocketfd,
      port = 7000;  // static port
      
  socklen_t clientLength;

  struct sockaddr_in server,
                     client;
  memset (&server, '\0', sizeof(server));

  struct serverData serverInfo;
  memset (serverInfo.clients.hostname, '\0', 32);
  memset (serverInfo.clients.username, '\0', 32);
  memset (serverInfo.clients.nickname, '\0', 9);
  memset (serverInfo.clients.nickname, '\0', 16);

  char buffer[SIZE];
  memset (buffer, '\0', SIZE);

  char * bufHead = buffer;


  if ((welcomeSocketfd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
    printf ("Error creating welcome socket\n");
    return -1;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons (port);

  if (bind (welcomeSocketfd, (struct sockaddr *) &server, sizeof (server)) == -1) {
    printf ("Error binding welcome socket, error: %d \n", errno);
    return -1;
  }

  if (listen (welcomeSocketfd, 10) == -1) {
    printf ("Error listening for connection, error: %d\n", errno);
    return -1;
  }
  printf ("listening...\n");

  clientLength = sizeof(client);

  if ((connectionSocketfd = accept (welcomeSocketfd, (struct sockaddr *) &client, 
      &clientLength)) < 0) {
    printf ("Error accepting client, error: %d, %d\n", errno, connectionSocketfd);
    return -1;
  }

  printf ("Client connected!");

  // If here, client connected, client will always first send empty packet
  
  if (read (connectionSocketfd, buffer, SIZE) == -1) {
    return -1; 
  }

  // Add new client info
  processMessage (buffer, &serverInfo, connectionSocketfd);
  sendMessage (buffer, connectionSocketfd);
  
// Now can procedd to 'regular' IRC communication
  while (1)
  {
    memset (buffer, '\0', SIZE);

    if (read (connectionSocketfd, buffer, SIZE) == -1) {
      printf ("Error reading from client, closing connection.\n");
      return -1; 
    }
    else
      printf ("received msg: %s\n", buffer);

    // TODO format and display message from client in buffer here
    processMessage (buffer, &serverInfo, connectionSocketfd);

    // TODO all communication logic here
  }
  return 0;
}
