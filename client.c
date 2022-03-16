/******
*
                  * client.c
*
*******/

#include "client_functions.h"

int main ()
{
  int connectionSocketfd,
      port = 7000;
  
  char buffer[SIZE]; // buffer for read and write
  memset (buffer, '\0', sizeof(buffer));

  struct sockaddr_in servAddr;
  memset (&servAddr, '\0', sizeof(servAddr));

  struct userInfo myInfo;

  getNick (myInfo.nickname, 9);

  if ((connectionSocketfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    return 1;

  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = inet_addr ("127.0.0.1");
  servAddr.sin_port = htons (port);

  if (inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr) == -1) 
    return -1;

  if (connect (connectionSocketfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1)
    return -1;   
  else
    printf("connected to the server..\n");

  getHostInfo (&myInfo);

  // Send initial message with client info, prompts welcome message from server
  sendMessage (&myInfo, buffer, connectionSocketfd);
  printf ("here line 43\n");
  if (read (connectionSocketfd, buffer, SIZE) == -1) {
    printf ("Error with initial cmd msg, err: %d\n", errno);
    return -1;
  }
  fputs (buffer, stdout);

  
  while (1) {
    memset (buffer, '\0', SIZE);
    printf ("> ");
    getstdin (buffer);
    createPacket (&myInfo, buffer);

    if (write (connectionSocketfd, buffer, SIZE) == -1) {
      printf ("No response from server, closing connection.\nerror: %d",
          errno);
      return -1;
    }

    int quit = 0;
    if (strcmp ("/quit", buffer) == 0)
      quit = 1;

    memset (buffer, '\0', SIZE);

    if (read (connectionSocketfd, buffer, SIZE) == -1) {
      printf ("No response from server, closing connection.\nerror: %d",
          errno);
      return -1;
    }

    fputs (buffer, stdout);
    if (quit) break;
  }
  return 0;
}
