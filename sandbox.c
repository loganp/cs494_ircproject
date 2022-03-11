#include <string.h>
#include <stdio.h>
#include <stdlib.h>



void createPacket (char * nick, char * hn, char * un, char * msg) 
{
  //message =  [ ":" prefix SPACE ] command [ params ] crlf

  char packet[256],
       cmd[16],
       param[32],
       mesg[256],
       * msgptr = mesg,
       * tok;
  memset (packet, '\0', 256);
  memset (cmd, '\0', 16);
  memset (param, '\0', 32);

  // Extract command, params, and message
  // Commmand if first word starts with '/' else only message
  // If command, only one param, then message

  strcpy (mesg, msg);

  tok = strtok (msg, " ");
  if (*tok == '/'){
    strcpy (cmd, tok);
    if ((tok = strtok (NULL, " ")) != NULL)  {
      strcpy (param, tok);
      while (*msgptr++ != ' ')
        ;
    }
    while (*msgptr++ != ' ')
      ;
  }
    

  printf (":%s!%s@%s %s %s :%s\r\n", nick, un, hn, cmd, param, msgptr); 
}




int main ()
{
 char * un = "logan",
      * hn = "xps9300",
      * nick = "log",
       msg[] = "/message server hello";

  createPacket (nick, hn, un, msg);
  return 0;
}
