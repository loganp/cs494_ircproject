/*****
* server.c
*
* simple TCP server on port 7000
******/

#include "server_functions.h"


int main (int argc, char ** argv)
{

  int welcomeSocketfd,
      connectionSocketfd,
      port = 7000,  // static port
      clientLength,
      size = 256;

  struct sockaddr_in server,
                     client;

  struct clientData cliData;

  char buffer[size],
       recBuffer[size],
       clientName;

  if ((welcomeSocketfd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
    printf ("Error creating welcome socket\n");
    return -1;
  }
  
  memset (&server, '0', sizeof(server));

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

  // If here, client connected
  // Get Client info  
  if (read (connectionSocketfd, buffer, size) == -1)
    return -1; 
  strcpy (cliData.hostname, buffer);

  if (read (connectionSocketfd, buffer, size) == -1)
    return -1; 
  strcpy (cliData.username, buffer);

  if (read (connectionSocketfd, buffer, size) == -1) {
    printf ("Error retrieving nick, error: %d\n", errno);
    return -1; 
  }
  strcpy (cliData.nickname, buffer);


  memset (buffer, '\0', size);
  
  snprintf (buffer, size, "%s connected\n", cliData.nickname);
  write (connectionSocketfd, buffer, size);

  while (1)
  {
    memset (buffer, '0', size);

    if (read (connectionSocketfd, buffer, size) == -1) {
      printf ("Error reading from client, closing connection.\n");
      return -1; 
    }

    // TODO format and display message from client in buffer here
    if (parseMessage (buffer, size) == -1) {
      write (connectionSocketfd, buffer, size);
      close (connectionSocketfd);
      close (welcomeSocketfd);
    }
    write (connectionSocketfd, buffer, size);

    // TODO all communication logic here

  }

  return 0;
}
