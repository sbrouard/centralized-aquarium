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
int parse(struct client_data* client, struct server* s){
  int i;
  for(i = 0; client->buffer[i] != '\n' || client->buffer[i] != ' '; ++i){
  }
  
  if (strncmp(client->buffer,"hello",i) == 0){
    hello(client,i,s);
  }

  else if (strncmp(client->buffer,"getFishes",i) == 0){
    getFishes(client,i,s);
  }

  else if (strncmp(client->buffer,"getFishesContinously",i) == 0){
    getFishesContinously(client,i);
  }

  else  if (strncmp(client->buffer,"log",i) == 0){
    log_out(client,i,s);
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
int read_client(struct client_data* client, struct server *s){
  while (recv(client->socket,&client->buffer[client->buffer_size],1,MSG_DONTWAIT) != -1){

      if (client->buffer[client->buffer_size] == '\n'){
	parse(client,s);
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


int init_server(struct server *t)
{
	t->socket = -1;
	t->nb_client = 0;
	t->client_list = NULL;
	init_terminal(&t->term);

	return 0;
}

int fd_to_read(server *serv,fd_set *set)
{
	int nfds = 0,i;

	if(serv->socket != -1)
	{
		nfds = serv->socket;
		FD_SET(serv->socket,set);
	}

	for(i=0;i<serv->nb_client;i++)
	{
		FD_SET(serv->client_list[i].socket,set);

		if(serv->client_list[i].socket > nfds)
			nfds = serv->client_list[i].socket;
	}

	return nfds + 1;
}

int read_server(server *serv,fd_set *set)
{
	int i;

	if(FD_ISSET(serv->socket,set))
	{
		//accept_client(serv);
	}

	for(i=0;i<serv->nb_client;i++)
	{
		if(FD_ISSET(serv->client_list[i].socket,set))
		{
			read_client(&serv->client_list[i]);
		}
	}

	return 0;
}

int affect_available_view(struct server *s, struct client_data* client){
  int j;
  char greet[12];
  for (j = 0; j < s->aqua.nb_views || s->aqua.views[j].client == AVAILABLE; ++j){}
  if (j ==  s->aqua.nb_views){
    return -1;
  }
  else {
    s->aqua.views[j].client = NOTAVAILABLE;
    client->id_view = j;
    sprintf(greet, "greeting N%d\n", j);
    send(client->socket,greet, strlen(greet),0);
    return 1;
  }
}

int hello(struct client_data* client, int indice, struct server * s){
  if  (client->buffer[indice] == ' '){ // Cas "in as N<ID>"
    int j;
    char * unknown = "Unknown command\n";
    char greet[12];
    if ( strncmp("in as N", &client->buffer[indice+1], 7)){ // chaine "in as" valable
      char c = client->buffer[indice+8];
      int num = atoi(&c);
      if (num >= 0 && num <= s->aqua.nb_views){ // numero de vue valable
	if(s->aqua.views[num].client == AVAILABLE){ // vue disponible
	  s->aqua.views[num].client = NOTAVAILABLE;
	  client->id_view = num;

 	  sprintf(greet, "greeting N%d\n", num);
	  send(client->socket,greet, strlen(greet),0);
	}
	else{ // vue non disponible, on en cherche une autre
	  
	  if (affect_available_view(s,client)== -1){
	    //aucune vue disponible
	    char * nogreet = "no greeting\n";
	    send(client->socket,nogreet, strlen(nogreet),0);
	  }
 	}
      }
      else { // numero de vue invalide
        send(client->socket,unknown, strlen(unknown),0);
	}
    }
    else { // chaine "in as" invalide
       send(client->socket,unknown, strlen(unknown),0);
    }
      
  }      
  else if (client->buffer[indice] == '\n'){ // Cas "hello\n", on cherche une vue disponible
    
    if (affect_available_view(s,client)== -1){
      //aucune vue disponible
      char * nogreet = "no greeting\n";
      send(client->socket,nogreet, strlen(nogreet),0);
    }
  }
  return 0;
}



int getFishes(struct client_data* client, int indice){return 0;}
int getFishesContinously(struct client_data* client, int indice){return 0;}
//Cette fonction deconnecte un client
int log_out(struct client_data* client, int indice){
  //Si on a "log out\n"
  if(strncmp(" out\n", &client->buffer[indice], 5)){
    s->aqua.views[client->id_view].client = AVAILABLE;
    client->id_view = NOVIEW;
    char * message = "bye\n"
      send(client->socket,message, strlen(message),0);
  }
  else {
    char * unknown = "Unknown command\n";
    send(client->socket,unknown, strlen(unknown),0);
  }
  return 0;
}

int ping(struct client_data* client, int indice){return 0;}
int status(struct client_data* client, int indice){return 0;}
int addFish(struct client_data* client, int indice){return 0;}
int delFish(struct client_data* client, int indice){return 0;}
int startFish(struct client_data* client, int indice){return 0;}

