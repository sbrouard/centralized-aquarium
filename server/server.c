#include "server.h"



//Crée et retourne la socket d'écoute du serveur
int initialization(int port){
  int socket_server;
  struct sockaddr_in server_ref;  

  socket_server = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if(socket_server == -1)
	{
		perror("socket()");
		return -1;
	}
  
  


  server_ref.sin_family = AF_INET;
  server_ref.sin_port = htons(port);
  server_ref.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(socket_server,(struct sockaddr*)&server_ref,sizeof(struct sockaddr_in)) == -1)
	{
		perror("bind()");
		return -1;
	}
  
  
  if(listen(socket_server,MAX_VIEWS) == -1)
	{
		perror("listen()");
		return -1;
	}

  return socket_server;
}



//Parse la première partie d'une commande d'un client qui termine par '\n'
int parse(struct client_data* client){
  int i;
  for(i = 0; client->buffer[i] != '\n' || client->buffer[i] != ' '; ++i){
  }
  
  if (strncmp(client->buffer,"hello",i) == 0){
    hello(client,i);
  }

  else if (strncmp(client->buffer,"getFishes",i) == 0){
    getFishes(client,i);
  }

  else if (strncmp(client->buffer,"getFishesContinously",i) == 0){
    getFishesContinously(client,i);
  }

  else  if (strncmp(client->buffer,"log",i) == 0){
    log_out(client,i);
  }
  
  else if (strncmp(client->buffer,"ping",i) == 0){
    ping(client,i);
  }
  else if (strncmp(client->buffer,"status",i) == 0){
    status(client,i);
  }
  else if (strncmp(client->buffer,"addFish",i) == 0){
    addFish(client,i);
  }
  else if (strncmp(client->buffer,"delFish",i) == 0){
    delFish(client,i);
  }
  else if (strncmp(client->buffer,"startFish",i) == 0){
    startFish(client,i);
  }
  else{
    char * unknown = "Unknown command\n";
    send(client->socket,unknown, strlen(unknown),0);
  }

}






//Lit sur la socket du client jusqu'au bout du bloc reçu et appelle parse si on reçoit '\n'
int read_client(struct client_data* client){
  while (recv(client->socket,&client->buffer[client->buffer_size],1,MSG_DONTWAIT) != -1){

      if (client->buffer[client->buffer_size] == '\n'){
	parse(client);
	client->buffer_size = 0;
      }else
	{
	++client->buffer_size;
	}

  }

  if(errno != EWOULDBLOCK)
    {
      perror("recv()");
      return -1;
    }
  return 0;
}


