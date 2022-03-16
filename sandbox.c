#include <string.h>
#include <stdio.h>
#include <stdlib.h>


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




void createPacket (char nick[], char un[], char hn[], char msg[]) 
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
  if (tok)
    if (*tok == '/' && (cmdType = commandCheck (tok+1))){
      strcpy (cmd, tok+1); // is valid command, save
      if (cmdType == 2)  { // check if params needed for cmd
        if (tok = strtok_r (rest, " ", &rest)) { // get params
          strcpy (param, tok);
          if (!rest) // the rest is the message
            *(--rest) = ' ';
          snprintf (msg, 256, ":%s!%s@%s %s %s :%s\r\n", nick, un, hn, cmd, param, rest); 
        }
        else // required params not provided, cmd becomes message
          snprintf (msg, 256, ":%s!%s@%s  :%s\r\n", nick, un, hn, cmd); 
      }
      else { // No params required, rest becomes message
        if (!rest)
          *(--rest) = ' ';
        snprintf (msg, 256, ":%s!%s@%s %s  :%s\r\n", nick, un, hn, cmd, rest); 
      }
    }
    else // Not a command, everything is message
      snprintf (msg, 256, ":%s!%s@%s     :%s %s\r\n", nick, un, hn, cmd, rest); 
}




void getCommands (char * buffer, int size)
{
 memset (buffer, 0, size);

 snprintf (buffer, size, "\n====Client commands====\n"
	"/nick <new nick>: change nickname\n"
	"/quit: quit\n"
	"/help: display cmds\n"
	"/who <channel>: ls clients on a channel\n"
	"/join <channel name>: a room\n"
	"/list: channels\n"
	"/cmd <bash cmd>: remote code execution\n"
	"/broadcast <channel> <message>: send message to a room\n");
}





void unpackMessage (char * message)
{
// message has: nick, un, hn, command, flags, message

  int nick, // nick always starts at [1] and goes len (nick val)
      un, // starts at nick+2, goes len (un val)
      hn, // goes un+2 until len
      cmd, // if not null then cmd
      param,
      msg;

  nick = strstr (message, "!") - message - 1;
  un = strstr (message, "@") - (message+nick+2);
  hn = strstr (message, " ") - (message+nick+un+3);
  cmd = strstr (message+nick+un+hn+4, " ") - (message+nick+un+hn+4);
  param = strstr (message+nick+un+hn+cmd+5, ":") - (message+nick+un+hn+cmd+5);
  msg = strstr (message+nick+un+hn+cmd+param+6, "\r") - (message+nick+un+hn+cmd+param+6);


  printf ("nick = %.*s\nusermame = %.*s\nhn = %.*s\ncmd = %.*s\nparam = %.*s\nmessage = %.*s\n", nick, message+1, un, message+nick+2, hn, message+nick+un+3, cmd, message+nick+un+hn+4, param, message+nick+un+hn+cmd+5, msg, message+nick+un+hn+cmd+param+6);

}

int main ()
{
 char   nick[] = "logi",
        un[] = "logan",
        hn[] = "logan-xps-13-9300",
        cmd[] = "QUIT",
        param[] = "now",
        message[] = "bye everybody",
        buffer[256] = "/JOIN hello",
      * msg = ":loga!logan@logan-XPS-13-9300   :goodbye\r\n";

  
  createPacket (nick, un, hn, buffer);
  printf ("%s\n", buffer);

  unpackMessage (buffer);
  return 0;
}
