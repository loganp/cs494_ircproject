#include "client_functions.h"



void getstdin (char * buffer) {
  fgets (buffer, sizeof(buffer), stdin);
  buffer[strcspn(buffer, "\n")] = 0;
}



void getHostInfo ( char * hn, char * un, int bufSize)
{
  memset (hn, '\0', bufSize);
  memset (un, '\0', bufSize);

  if (gethostname (hn, bufSize) == -1) {
    hn[0] = '\0';
  }

  uid_t uid = geteuid();
  struct passwd * pw = getpwuid (uid);
  if (!pw) {
    un = '\0';
  }
  else
    strcpy (un, pw->pw_name);
}



void getNick (char * buffer, int bufSize) 
{
  printf ("Enter Nick (9 char max): ");
  memset (buffer, '\0', bufSize);
  getstdin (buffer);
}




void createPacket (char * nick, char * hn, char * un, char * msg, int size) 
{
  //message =  [ ":" prefix SPACE ] command [ params ] crlf

  char packet[256],
       cmd[16],
       params[32],
       mesg[256],
       * tok;
  memset (packet, '\0', 256);
  memset (cmd, '\0', 16);
  memset (params, '\0', 32);
  memset (mesg, '\0', 256);

  // Extract command, params, and message
  // Commmand if first word starts with '/' else only message
  // If command, only one param, then message

  tok = strtok (msg, " ");
  if (tok == '/'){
    strcpy (cmd, tok);
    if ((tok = strtok (NULL, msg)) != NULL) 
      strcpy (param, tok);
    

  snprintf (packet, 512, ":%s!%s@%s %s %s :%s\r\n", nick, un, hn, cmd, parms, mesg); 
