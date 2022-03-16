#include "client_functions.h"

int SIZE = 512;


int stralnum (char * str)
{
  for (int i = 0; i < 9 && (str[i] != '\0'); i++) {
    if (!((int)str[i] >= 48 && (int)str[i] <= 57)
          && !((int)str[i] >= 65 && (int)str[i] <= 90)
          && !((int)str[i] >= 97 && (int)str[i] <= 122))
      return -1;
  }
  return 0;
}

void getstdin (char * buffer) {
  fgets (buffer, sizeof(buffer), stdin);
  buffer[strcspn(buffer, "\n")] = 0;
}



void getHostInfo (struct userInfo * info)
{
  memset (info->hostname, '\0', 32);
  memset (info->username, '\0', 16);

  if (gethostname (info->hostname, 32) == -1) {
    info->hostname[0] = '\0';
  }

  uid_t uid = geteuid();
  struct passwd * pw = getpwuid (uid);
  if (!pw) {
    *info->username = '\0';
  }
  else
    strcpy (info->username, pw->pw_name);
}



void getNick (char * buffer, int bufSize) 
{
  do {
    printf ("Enter Nick (9 char max): ");
    memset (buffer, '\0', bufSize);
    getstdin (buffer);
  } while (stralnum (buffer));
}


/* Checks whether input is a valid command
   Return: 1 valid command and no param
           2: valid command w/ params req'd
          -1: invalid command
*/
int commandCheck (char * message)
{
  char noParamCmd[] = "QUIT DIE HELP CMD LIST NICK WHO JOIN PART MSG",
       * tok;

  int i = 1;
  tok = strtok (noParamCmd, " ");
  while (tok) {
    if (strcmp (tok, message) == 0)
      return i;
    tok = strtok (NULL, " ");
    i++;
  }
  return 0;
}
  
  /*
  1.  /QUIT <reason>: quit\n"
  2.  /DIE: shutdown server\n"
  3.  /HELP: display cmds\n"
  4.  /CMD <bash cmd>: remote code execution\n"
  5.  /LIST: available channels\n"
  6.  /NICK <new nick>: change nickname\n"
  7.  /WHO <channel>: ls clients on a channel\n"
  8.  /JOIN <channel>: a room\n"
  9.  /PART <channel>: leave room\n"
  10. /MSG <#channel> <message>:
  */



void createPacket (struct userInfo * info, char msg[]) 
{
  //message =  [ ":" prefix SPACE ] command [ params ] crlf

  char packet[256],
       cmd[16],
       param[32],
       mesg[256],
       * tok,
       * rest;
  int cmdType = 0;

  memset (packet, '\0', 256);
  memset (cmd, '\0', 16);
  memset (param, '\0', 32);
  memset (mesg, '\0', 256);

  // Extract command, params, and message
  // Commmand if first word starts with '/' else only message
  // If command, only one param, then message

  strcpy (mesg, msg);
  rest = mesg;
// /boop beep message
  tok = strtok_r (rest, " ", &rest);   // Extract first token should be cmd
  if (tok) {
    if (*tok == '/' && (cmdType = commandCheck (tok+1))){
      strcpy (cmd, tok+1); // is valid command, save
      if (cmdType == 2)  { // check if params needed for cmd
        if ((tok = strtok_r (rest, " ", &rest))) { // get params
          strcpy (param, tok);
          if (!rest) // the rest is the message
            *(--rest) = ' ';
          snprintf (msg, 256, ":%s!%s@%s %s %s :%s\r\n", 
            info->nickname, info->username, info->hostname, cmd, param, rest); 
        }
        else // required params not provided, cmd becomes message
          snprintf (msg, 256, ":%s!%s@%s  :%s\r\n", 
            info->nickname, info->username, info->hostname, cmd); 
      }
      else { // No params required, rest becomes message
        if (!rest)
          *(--rest) = ' ';
        snprintf (msg, 256, ":%s!%s@%s %s  :%s\r\n", 
          info->nickname, info->username, info->hostname, cmd, rest); 
      }
    }
    else // Not a command, everything is message
      snprintf (msg, 256, ":%s!%s@%s     :%s %s\r\n", 
        info->nickname, info->username, info->hostname, cmd, rest); 
  }
  else 
      snprintf (msg, 256, ":%s!%s@%s     :\r\n", 
        info->nickname, info->username, info->hostname); 
}



int sendMessage (struct userInfo * info, char * buffer, const int socket)
{
  
  createPacket (info, buffer);
  if (write(socket, buffer, SIZE) == -1)
    return -1;
  
  return 0;
}