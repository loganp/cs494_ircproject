#include "server_functions.h"


int MAX_ROOMS = 16;

int SIZE = 512;



int sendMessage (char * buffer, const int socket)
{
  if (write(socket, buffer, SIZE) == -1){
    printf ("Error with initial cmd msg, err: %d\n", errno);
    return -1;
  }
}




void getCommands (char * buffer)
{
 //memset (buffer, '\0', SIZE);

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
    printf("cmd:: %.*s\n",len_cmd, message+len_nick+len_un+len_hn+4);
    printf("param:: %.*s\n",len_param, message+len_nick+len_un+len_hn+len_cmd+5);
    printf("rest:: %.*s\n",len_msg, message+len_nick+len_un+len_hn+len_cmd+len_param+6);

  // Designed for single client, multiple TODO
  if (!*(int*)cdPtr->nickname) { // Client DNE, add new
    data->activeClients++;
    strncpy (cdPtr->nickname, message+1, len_nick);  
    strncpy (cdPtr->username, message+len_nick+2, len_un);
    strncpy (cdPtr->hostname, message+len_nick+len_un+3, len_hn);
    snprintf (message, SIZE, "Welcome %s!! Here are the supported commands:\n", 
                cdPtr->nickname);
    getCommands (message+strlen(message)-2); // Append help to welcome command
    sendMessage (message, socket);  // Send welcome message
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
      clientQuit (originCli, serverInfo);
      printf("Client say quit\n");
      break;

    case 2 :
      // DIE
      //serverShutDown (originClient, serverInfo, socket);
      printf("Client say shut that server down\n");
      break;

    case 3 :
      // HELP
      getCommands (pcommand);
      sendMessage (pcommand, socket);
      printf("Client say help\n");
      break;

    case 4 :
      // CMD  -- Maybe not actually possible
      printf("Client say rce\n");
      break;

    case 5 :
      // LIST
      listRooms (serverInfo, socket);
      printf("Client say LIST\n");
      break;

    case 6 :
      // NICK
      changeNick (pcommand, originCli);
      printf("Client say NICK\n");
      break;

    case 7 :
      // WHO
      who (pcommand, originCli, serverInfo, socket);
      printf("Client say WHO\n");
      break;

    case 8 :
      // JOIN
      joinRoom (pcommand, originCli, serverInfo);
      break;

    case 9 :
      // PART
      part (pcommand, originCli, serverInfo, socket);
      printf("Client say PART\n");
      break;
      
    case 10 :
      // MSG
      broadcast (pcommand, originCli, serverInfo, socket);
      printf("Client say PART\n");
      break;
  }
}






void clientQuit (struct clientData * originCli, struct serverData * serverInfo)
{
  // remove from lists
  memset (originCli->hostname, '\0', 32); 
  memset (originCli->username, '\0', 32);
  memset (originCli->nickname, '\0', 9);

  for (int i = 0; i < serverInfo->roomCount; i++) {
    serverInfo->rooms[i].members = NULL;
  }

} 



void joinRoom (struct prsdCommand * cmd, struct clientData * client,
      struct serverData * server)
{
  int added = 0;
  for (int i = 0; i < server->roomCount; ++i) {
    if (strcmp (cmd->params, server->rooms[i].name) == 0) {
      // Replace with for loop for multiple members
      if (client == server->rooms[i].members) {
        server->rooms[i].members = client;
        added = 1;
      }
        break;
    }
  }
  // Not added, create room
  if (!added && server->roomCount < 16) {
    strcpy (server->rooms[server->roomCount].name, cmd->params);
    strcpy (server->rooms[server->roomCount].name, client->nickname);
    server->roomCount++;
  }
  
}



void listRooms (struct serverData * serverInfo, const int socket)
{
  char buffer[SIZE];
  snprintf (buffer, sizeof buffer, "==== Channels ====\n");
  for (int i = 0; i < serverInfo->roomCount; i++){
    snprintf (buffer+ strlen(buffer), "%s\n", serverInfo->rooms[i].name);
  }
  sendMessage (buffer, socket);
}



void changeNick (struct prsdCommand * cmd, struct clientData * client)
{
  // Only the client record needs to be updated
  memset (client->nickname, '\0', 9);
  strcpy (client->nickname, cmd->params);

}



void who (struct prsdCommand * cmd, struct clientData * client,
                struct serverData * server, const int socket)
{
  char buffer[SIZE];
  memset (buffer, '\0', 64);

  for (int i = 0; i < server->roomCount; ++i) {
    if (strcmp (cmd->params, server->rooms[i].name) == 0 
       && server->rooms[i].members == client) {
      // Replace with loop when multiple clients are connected
      snprintf (buffer, 64, "Members of #%s\n------------\n%s\n", 
                  cmd->params, server->rooms[i].members->nickname);
      sendMessage (buffer, socket);
    }
  }
}



void part (struct prsdCommand * cmd, struct clientData * client,
                struct serverData * server, const int socket)
{
  char * buffer[SIZE];
  memset (buffer, '\0', SIZE);

  for (int i = 0; i < server->roomCount; ++i) {
    if (strcmp (cmd->params, server->rooms[i].name) == 0) {
      // Replace with for loop for multiple members
      if (client ==  server->rooms[i].members) 
        server->rooms[i].members = NULL;
        snprintf (buffer, SIZE, "You have left channel #%s\n", server->rooms[i].name);
        sendMessage (buffer, socket);
      break;
    }
  }
}



void broadcast (struct prsdCommand * cmd, struct clientData * client,
                struct serverData * server, const int socket)
{
  char * buffer[SIZE];
  memset (buffer, '\0', SIZE);

  for (int i = 0; i < server->roomCount; ++i) {
    if (strcmp (cmd->params, server->rooms[i].name) == 0) {
      if (client ==  server->rooms[i].members) 
      // Client is member of group, can send message
        snprintf (buffer, SIZE, "#%s: %s\n", server->rooms[i].name, cmd->message);
        sendMessage (buffer, socket);
      break;
    }
  }
}                