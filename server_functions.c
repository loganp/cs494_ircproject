#include "server_functions.h"


int MAX_ROOMS = 16;

int SIZE = 512;



/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t strlcpy(char *dst, const char *src, size_t siz)
{
  char *d = dst;
  const char *s = src;
  size_t n = siz;

  // Copy as many bytes as will fit 
  if (n != 0 && --n != 0) {
    do {
      if ((*d++ = *s++) == 0)
        break;
    } while (--n != 0);
  }

  // Not enough room in dst, add NUL and traverse rest of src
  if (n == 0) {
    if (siz != 0)
      *d = '\0';        
    while (*s++)
      ;
  }
  return(s - src - 1);   
}



int sendMessage (char * buffer, const int socket)
{
  if (write(socket, buffer, SIZE) == -1){
    printf ("Error with initial cmd msg, err: %d\n", errno);
    return -1;
  }
}




int parseMessage (char * msg, int bufLen)
{
  if (msg[0] == '/') {
    // Command message
    if (strcmp (msg, "quit") == 0) {
      strlcpy (msg, "Quitting\n", sizeof(msg));
      return -1;
    }
  }
  
  snprintf (msg, bufLen, "SERVER: Good point, too true\n");
  
  return 0;
}




void getCommands (char * buffer)
{
 memset (buffer, '\0', SIZE);

 snprintf (buffer, SIZE, "\n====Client commands====\n"
	"/NICK <new nick>: change nickname\n"
        "/QUIT <reason>: quit\n"
        "/DIE: shutdown server\n"
	"/HELP: display cmds\n"
	"/WHO <channel>: ls clients on a channel\n"
	"/JOIN <channel>: a room\n"
        "/PART <channel>: leave room\n"
	"/LIST: available channels\n"
	"/CMD <bash cmd>: remote code execution\n"
	"/MSG <#channel> <message>: broadcast message to a room\n");
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



void processMessage (char * message, struct serverData * data, const int socket)
{
// message has: nick, un, hn, command, flags, message

  struct clientData * cdPtr = &data->clients;  // Used to iterate once multiple clients

  int len_nick, // nick always starts at [1] and goes len (nick val)
      len_un, // starts at nick+2, goes len (un val)
      len_hn, // goes un+2 until len
      len_cmd, // if not null then cmd
      len_param,
      len_msg;

  len_nick = strstr (message, "!") - message - 1;

  len_un = strstr (message, "@") - (message+len_nick+2); 

  len_hn = strstr (message, " ") - (message+len_nick+len_un+3);

  len_cmd = strstr (message+len_nick+len_un+len_hn+4, " ") 
                - (message+len_nick+len_un+len_hn+4);

  len_param = strstr (message+len_nick+len_un+len_hn+len_cmd+5, ":") 
                - (message+len_nick+len_un+len_hn+len_cmd+5);

  len_msg = strstr (message+len_nick+len_un+len_hn+len_cmd+len_param+6, "\r")
                - (message+len_nick+len_un+len_hn+len_cmd+len_param+6);


    // {length of field}, {start index of field}
    printf("nick:: %.*s\n", len_nick, message+1); 
    printf("un:: %.*s\n",len_un, message+len_nick+2);
    printf("hn:: %.*s\n",len_hn, message+len_nick+len_un+3); 
    //printf("nick:: %.*%s\n",len_cmd, message+len_nick+len_un+len_hn+4, 
    //printf("nick:: %.*%s\n",len_param, message+len_nick+len_un+len_hn+len_cmd+5, 
    //printf("nick:: %.*%s\n",len_msg, message+len_nick+len_un+len_hn+len_cmd+len_param+6);

  // Designed for single client, multiple TODO
  if (!*(int*)cdPtr->nickname) { // Client DNE, add new
    strncpy (cdPtr->nickname, message+1, len_nick);  
    strncpy (cdPtr->username, message+len_nick+2, len_un);
    strncpy (cdPtr->hostname, message+len_nick+len_un+3, len_hn);
    snprintf (message, SIZE, "Welcome %s!! Here are the supported commands:\n", 
                cdPtr->nickname);
    getCommands (message+strlen(message)-2); // Append help to welcome command
    sendMessage (message, SIZE);  // Send welcome message
  } 
  else { // Client exists, process message and respond
    if (len_cmd > 1) {
      // If is a command, extract command, params, and message
      struct prsdCommand command;

      strncpy (command.command, message+len_nick+len_un+len_hn+4, len_cmd);
      strncpy (command.params, message+len_nick+len_un+len_hn+len_cmd+5, len_param);
      strncpy (command.message, message+len_nick+len_un+len_hn+len_cmd+len_param+6, len_msg);

      // Process command

      doCommand (&command, cdPtr, data, socket);
    }
    else {
    // else write message to all
      char buffer[SIZE];
      memset (buffer, '\0', SIZE);

      snprintf (buffer, SIZE, "%s: %.*s\n", cdPtr->nickname,
        len_msg, message+len_nick+len_un+len_hn+len_cmd+len_param+6);
      
      sendMessage (buffer, socket);
    }
  }
}




void doCommand (struct prsdCommand * pcommand, struct clientData * originCli, 
                    struct serverData * serverInfo, const int socket)
{
  int commandIndex = commandCheck (pcommand->command);

  switch (commandIndex) {
    case 1 :
      // QUIT
      //clientQuit (prsdCommand->message, originClient, serverInfo, socket);
      printf("Client say quit\n");
      break;

    case 2 :
      // DIE
      //serverShutDown (originClient, serverInfo, socket);
      printf("Client say shut that server down\n");
      break;

    case 3 :
      // HELP
      //getCommands (buffer, SIZE);
      //sendMessage (buffer, socket);
      printf("Client say help\n");
      break;

    case 4 :
      // CMD
      printf("Client say rce\n");
      break;

    case 5 :
      // LIST
      printf("Client say LIST\n");
      break;

    case 6 :
      // NICK
      printf("Client say NICK\n");
      break;

    case 7 :
      // WHO
      printf("Client say WHO\n");
      break;

    case 8 :
      // JOIN
      printf("Client say JOIM\n");
      break;

    case 9 :
      // PART
      printf("Client say PART\n");
      break;
      
    case 10 :
      // MSG
      printf("Client say PART\n");
      break;

    default :  
      printf ("It shouldn't be like this but it do"); 
  }
}






void clientQuit (char * mesg, struct clientData * originCli, 
                  struct serverData * serverInfo, const int socket)
{
  
} 

