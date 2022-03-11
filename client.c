/******
*
                  * client.c
*
*******/

#include "client_functions.h"

int main (int argc, char ** argv)
{
  int connectionSocketfd,
      port = 7000,
      size = 256;
  
  char buffer[size],
       hn[32],
       un[32];

  struct sockaddr_in servAddr;
  
  struct hostent * server;


  getNick (buffer, size);

  if ((connectionSocketfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    return 1;

  memset (&servAddr, '0', sizeof(servAddr));
  
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = inet_addr ("127.0.0.1");
  servAddr.sin_port = htons (port);

  if (inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr) == -1) 
    return -1;


  if (connect (connectionSocketfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1)
    return -1;   
  else
    printf("connected to the server..\n");

  getHostInfo (hn, un, 32);

  // Send hostname
  if (write(connectionSocketfd, hn, size) == -1)
    return -1;

  // Send username
  if (write(connectionSocketfd, un, size) == -1)
    return -1;

  // Send nickname
  if (write(connectionSocketfd, buffer, size) == -1)
    return -1;

  memset (buffer, 0, sizeof(buffer));

  if (read (connectionSocketfd, buffer, size) == -1) {
    // This is the final piece of welcome init (server broadcasts welcome msg to all
    printf ("Error with welcome msg, err: %d\n", errno);
    return -1;
    }

  fputs (buffer, stdout);

  
  while (1) {
    memset (buffer, 0, size);
    printf ("> ");
    getstdin (buffer);

    if (write (connectionSocketfd, buffer, size) == -1)
      return -1;

    memset (buffer, 0, size);

    if (read (connectionSocketfd, buffer, size) == -1) {
      printf ("No response from server, closing connection.\nerror: %d", errno);
      return -1;
    }


    if (strcmp ("/quit", buffer) == 0)
      break;

    fputs (buffer, stdout);

  }
  


  return 0;
}
