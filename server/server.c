#include "server.h"

#define TEST {printf("test\n");fflush(0);}

int init_client(struct client_data *c, int sock)
{
  c->socket = sock;
  c->id_view = -1;//PAS ENCORE PRIS EN COMPTE
  c->buffer_size = 0;
}




//Parse la première partie d'une commande d'un client
int parse(struct client_data* client, struct server* s){
int i;
  for(i = 0; client->buffer[i] != '\n' && client->buffer[i] != ' '; ++i){
  }
  if (strncmp(client->buffer,"hello",i) == 0){
    hello(client,i,s);
  }

  else if (strncmp(client->buffer,"getFishes",i) == 0){
    getFishes(client,i,s);
  }

  else if (strncmp(client->buffer,"getFishesContinously",i) == 0){
    getFishesContinously(client,i,s);
  }

  else  if (strncmp(client->buffer,"log",i) == 0){
    log_out(client,i,s);
  }
  
  else if (strncmp(client->buffer,"ping",i) == 0){
    ping(client,i);
  }
  else if (strncmp(client->buffer,"status",i) == 0){
    status(client,i,s);
  }
  else if (strncmp(client->buffer,"addFish",i) == 0){
    addFish(client,i,s);
  }
  else if (strncmp(client->buffer,"delFish",i) == 0){
    delFish(client,i,s);
  }
  else if (strncmp(client->buffer,"startFish",i) == 0){
    startFish(client,i,s);
  }
  else{
    char * unknown = "NOK : commande introuvable\n";
    send(client->socket,unknown, strlen(unknown),0);
  }

}

int remove_client(struct client_data* client, struct server *s)
{
	int i;

	close(client->socket);

	for(i=0;i<s->nb_client;i++)
	{
		if(client == &s->client_list[i])
			break;
	}

	if(i == s->nb_client)
		return -1;

	s->nb_client--;

	for(;i<s->nb_client;i++)
		s->client_list[i] = s->client_list[i+1];

	return 0;
}




//Lit sur la socket du client jusqu'au bout du bloc reçu et appelle parse si on reçoit '\n'
int read_client(struct client_data* client, struct server *s){
	int res;

	res = recv(client->socket,&client->buffer[client->buffer_size],1,MSG_DONTWAIT);


	
  while (res != -1){

	if(res == 0)
	{
		printf("client disconnected\n");
		remove_client(client,s);
		return 0;
	}

      if (client->buffer[client->buffer_size] == '\n'){
	++client->buffer_size;
	parse(client,s);
	client->buffer_size = 0;
      }else
	{
	++client->buffer_size;
	}
	res = recv(client->socket,&client->buffer[client->buffer_size],1,MSG_DONTWAIT);
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

	return 0;
}

int fd_to_read(struct server *serv,fd_set *set)
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

int read_server(struct server *serv,fd_set *set)
{
	int i;
	struct sockaddr_in client_in;
	int size;

	if(FD_ISSET(serv->socket,set))
	{
		printf("client accepted\n");
		if(serv->client_list == NULL)
		{
			serv->client_list = malloc(sizeof(struct client_data)*serv->conf.max_client);
		}
		init_client(&serv->client_list[serv->nb_client],accept(serv->socket,(struct sockaddr*)&client_in,&size));
		serv->nb_client++;
	}

	for(i=0;i<serv->nb_client;i++)
	{
		if(FD_ISSET(serv->client_list[i].socket,set))
		{
		  read_client(&serv->client_list[i], serv);
		}
	}

	return 0;
}




//Traduit une abscisse globale en abscisse locale % 
int trad_coorx(struct aquarium *a,int view,int x){
  int newx = x;
  int i;
  for (i = 0; i < view; ++i){
    newx = newx - a->views[i].pos.x;
  }

  newx = (newx*100)/(a->views[view].size.width);
  return newx;
 }

//Traduit une ordonnee globale en ordonnee locale % 
int trad_coory(struct aquarium *a,int view, int y){
  int newy = y;
  int i;
  for (i = 0; i < view; ++i){
    newy = newy - a->views[i].pos.y;
  }

  newy = (newy*100)/(a->views[view].size.height);
  return newy;
 }

//verifie si un poisson est dans une vue
int fishIsInView(struct fish* f, struct view* v){
  if (f->pos.x >= v->pos.x && f->pos.x <= v->pos.x + v->size.width && f->pos.y >= v->pos.y && f->pos.y <= v->pos.y + v->size.height)
    return 1;
  else 
    return 0;
}


//remplit par effet de bord un tableau avec les indices des poissons de la vue
int findFishesOfView(struct aquarium* a, int view, int *tabfish ){
  int len = 0;
  int i;
  for (i = 0; i < a->nb_fishes; ++i){
    
    if (fishIsInView(&a->fishes[i],&a->views[view])){
      ++len;
      tabfish = realloc(tabfish,len);
      tabfish[len-1] = i;
    }
	 }
  return len;
 
}



//utilisee dans hello() pour factoriser l'affectation d'une vue a un client
int affect_available_view(struct server *s, struct client_data* client){
  int j;
  char greet[12];
  for (j = 0; j < s->aqua.nb_views && s->aqua.views[j].client != AVAILABLE; ++j){}
  if (j ==  s->aqua.nb_views){
    return -1;
  }
  else {
    s->aqua.views[j].client = NOTAVAILABLE;
    client->id_view = j;
    sprintf(greet, "greeting N%d\n", j+1);
    send(client->socket,greet, strlen(greet),0);
    return 1;
  }
}

int hello(struct client_data* client, int indice, struct server * s){
  if  (client->buffer[indice] == ' '){ // Cas "in as N<ID>"
    int j;
    char * unknown = "NOK : commande introuvable\n";
    char greet[12];
    if ( strncmp("in as N\n", &client->buffer[indice+1], 8)){ // chaine "in as" valable
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


//auxiliaire pour getFishes et getFishesContinously qui envoie la liste des poissons de la vue
int sendFishesOfView(struct client_data* client, struct server* s){
  char msg[4096];
  int * tabfish = malloc(sizeof(int));
  int len = findFishesOfView(&(s->aqua), client->id_view, tabfish);
      if (len > 0){
	sprintf(msg, "list");
	for (int i = 0; i < len; i++){
	  sprintf(msg+4, " [%s at %dx%d, %dx%d, %d]", s->aqua.fishes[tabfish[i]].name,trad_coorx(&s->aqua,client->id_view,s->aqua.fishes[tabfish[i]].pos.y),trad_coory(&s->aqua,client->id_view,s->aqua.fishes[tabfish[i]].pos.y), s->aqua.fishes[tabfish[i]].size.width,  s->aqua.fishes[tabfish[i]].size.height, DEFAULT_DURATION);
	}
	
	strcat(msg, "\n");
	send(client->socket, msg, strlen(msg),0);
	
	
      }
      else{
	send(client->socket, "No fish\n", strlen("No fish\n"),0);
      }


      free(tabfish);

      return 0;
}



int getFishes(struct client_data* client, int indice, struct server* s){
  if (client->buffer[indice] != '\n'){
    char * unknown = "NOK : commande introuvable\n";
    send(client->socket,unknown, strlen(unknown),0);
  }
  
  else 
    {
      sendFishesOfView(client,s);
      return 0;


    }



return 0;
}


int getFishesContinously(struct client_data* client, int indice, struct server * s){
   if (client->buffer[indice] != '\n'){
     char * unknown = "NOK : commande introuvable\n";
     send(client->socket,unknown, strlen(unknown),0);
   }
   else {
     int t = s->conf.fish_update_interval;
     while (1){
       sleep(t);
       sendFishesOfView(client,s);
     }
     

   }


return 0;
}





//Cette fonction deconnecte un client
int log_out(struct client_data* client, int indice, struct server *s){
  //Si on a "log out\n"
  if(strncmp(" out\n", &client->buffer[indice-1], 5)){
    s->aqua.views[client->id_view].client = AVAILABLE;
    client->id_view = NOVIEW;
    char * message = "bye\n";
      send(client->socket,message, strlen(message),0);
  }
  else {
    char * unknown = "NOK : commande introuvable\n";
    send(client->socket,unknown, strlen(unknown),0);
  }
  return 0;
}

int ping(struct client_data* client, int indice){
  char msg[1024];
  sprintf(msg, "pong%s", &client->buffer[indice]);
  send(client->socket,msg, strlen(msg),0);
  return 0;
}


//retourne l'indice dans le tableau des poissons si le nom du poisson est déjà pris, 1 sinon
int alreadyExistsFish(struct server *s, const char * name){
  
  int i;
  for (i = 0; i < s->aqua.nb_fishes; ++i){
    if (strcmp(name, s->aqua.fishes[i].name) == 1){
      return 0;
    }

  }

  return i;
    

}



int addFish(struct client_data* client, int indice, struct server * s){
  char * unknown = "NOK : commande introuvable\n";



  if (s->aqua.nb_fishes >= MAX_FISHES-1){
      send(client->socket,"NOK\n",4,0);
    return UNKNOWN_COMMAND;


    }


  if (client->buffer[indice] != ' '){ 
    send(client->socket,unknown, strlen(unknown),0);
    return UNKNOWN_COMMAND;
  }

    struct fish f;
    int i;
    for (i = indice+1; client->buffer[i] != ' ' && i < client->buffer_size; ++i){}

    int name_length = i-(indice+1);
    if ( i >=  client->buffer_size ) { //Cas addFish + espace + \n
            send(client->socket,unknown, strlen(unknown),0);
	    return UNKNOWN_COMMAND;
    }
    else if (name_length <= NAME_LENGTH)  { // Vérification que le nom du poisson rentre dans la chaine de caractères
      strncpy(f.name, &client->buffer[indice+1], name_length);
      int exist = alreadyExistsFish(s,f.name);
      if (exist){ // Cas nom poisson deja existant
	send(client->socket, "NOK\n", 4,0);
	return UNKNOWN_COMMAND;
      } 
	printf("nom poisson:%s\n",f.name);
    }
    else {
      send(client->socket, "NOK\n", 4,0);
	return UNKNOWN_COMMAND;
    }
    
    if(strncmp(" at ", &client->buffer[i], 4) == 0){
TEST      
i += 4;
    }
    else {
      send(client->socket,unknown, strlen(unknown),0);
      return UNKNOWN_COMMAND;
    }
    

    //PARSING COORDONNEES DE POSITION
    int j = 0;
    int nb = 1;
    char msg[3] = { '\0', '\0', '\0'};
    int pos;
    for ( ;client->buffer[i] != ',' && i < client->buffer_size; ++i){
      if (client->buffer[i] != 'x' && j < 3){
	msg[j] = client->buffer[i];
	++j;
      }
      else if (client->buffer[i] == 'x' && j != 0 && nb == 1){
	pos = atoi(msg);
	++nb;
	if ( pos >= 0 && pos <= 100){
	  f.pos.x = pos;
	  j = 0;
	  for (int c = 0; c < 3; ++c){
	    msg[c] = '\0';
	  }
	}
      }

      else if (nb == 2 && j < 3){
	msg[j] = client->buffer[i];
	++j;
      }

      else if (nb == 2 && client->buffer[i+1] == ',' && j != 0){
	pos = atoi(msg);
	if ( pos >= 0 && pos <= 100){
	  f.pos.y = pos;
      }

      else{
	send(client->socket,unknown, strlen(unknown),0);
	return UNKNOWN_COMMAND;
	}
       
	
    }
}
    if (i >= client->buffer_size){
      	send(client->socket,unknown, strlen(unknown),0);
	return UNKNOWN_COMMAND;

    }


    //PARSING DIMENSIONS
    

    j = 0;
    nb = 1;
    char msg2[3] = { '\0', '\0', '\0'};
    for ( ;client->buffer[i] != ',' && i < client->buffer_size; ++i){
      if (client->buffer[i] != 'x' && j < 3){
	msg2[j] = client->buffer[i];
	++j;
      }
      else if (client->buffer[i] == 'x' && j != 0 && nb == 1){
	pos = atoi(msg2);
	++nb;
	if ( pos >= 0 && pos <= 100){
	  f.size.width = pos;
	  j = 0;
	  for (int c = 0; c < 3; ++c){
	    msg2[c] = '\0';
	  }
	}
      }

      else if (nb == 2 && j < 3){
	msg2[j] = client->buffer[i];
	++j;
      }

      else if (nb == 2 && client->buffer[i+1] == ',' && j != 0){
	pos = atoi(msg2);
	if ( pos >= 0 && pos <= 100){
	  f.size.height = pos;
      }

      else{
	send(client->socket,unknown, strlen(unknown),0);
	return UNKNOWN_COMMAND;
	}
       
	
    }
}
    if (i >= client->buffer_size){
      	send(client->socket,unknown, strlen(unknown),0);
	return UNKNOWN_COMMAND;

    }

    //PARSING MOBILITE
    if ( client->buffer[i+1] != ' '){
      	send(client->socket,unknown, strlen(unknown),0);
	return UNKNOWN_COMMAND;
    }
    
    
    char mobility[NAME_LENGTH];
    j = 0;
    
    for (i+=2; client->buffer[i] != '\n' && j < NAME_LENGTH; ++i){
      mobility[j] = client->buffer[i];
      ++j;
    }
    
    if (j >= NAME_LENGTH){
      send(client->socket,unknown, strlen(unknown),0);
      return UNKNOWN_COMMAND;	
    }
      
    strcpy(f.mobility, mobility);
    

    s->aqua.fishes[s->aqua.nb_fishes] = f;
    ++s->aqua.nb_fishes;
   
    
    

return 0;
}

int delFish(struct client_data* client, int indice, struct server * s){

    char * unknown = "NOK : commande introuvable\n";
  if (client->buffer[indice] != ' '){ 
    send(client->socket,unknown, strlen(unknown),0);
    return UNKNOWN_COMMAND;
  }
  else {
    struct fish f;
    int i;
    for (i = indice+1; client->buffer[i] != '\n'; ++i){}

    int name_length = i-(indice+1);
    
    if (name_length <= NAME_LENGTH)  { // Vérification que le nom du poisson rentre dans la chaine de caractères
      strncpy(f.name, &client->buffer[indice+1], name_length);
      int exist = alreadyExistsFish(s,f.name);
      if (!exist){ // Cas poisson inexistant
	send(client->socket, "NOK : Poisson inexistant\n", 25,0);
	return UNKNOWN_COMMAND;
      }
      else{
	for (int j = exist; j < s->aqua.nb_fishes-1 ; ++j){
	  s->aqua.fishes[j] = s->aqua.fishes[j+1];
	}
	--s->aqua.nb_fishes;
      }
    }
    else {
      send(client->socket, "NOK\n", 4,0);
	return UNKNOWN_COMMAND;
    }
  }
  return 0;
}
int startFish(struct client_data* client, int indice, struct server * s){
  char * unknown = "NOK : commande introuvable\n";
  if (client->buffer[indice] != '\n'){ 
    send(client->socket,unknown, strlen(unknown),0);
    return UNKNOWN_COMMAND;
  }
  else {
    
    send(client->socket,"OK\n",3,0);
  }
  


return 0;
}


//envoie l'etat de la connexion (les poissons de la vue) avec le serveur au client
int status(struct client_data* client, int indice, struct server* s){
  if (client->buffer[indice] != '\n'){
    char * unknown = "NOK : commande introuvable\n";
    send(client->socket,unknown, strlen(unknown),0);
  }

  else {
    char msg1[256];
    char msg2[256];
    int i;
    int * tabfish = malloc(sizeof(int));
    int len = findFishesOfView(&(s->aqua), client->id_view, tabfish);
    sprintf(msg1, "OK : Connecte au controleur, %d poisson(s) trouve(s)\n", len);
    send(client->socket,msg1, strlen(msg1),0);
    for (i = 0 ; i < len; ++i){
      sprintf(msg2, "Fish %s at %dx%d,%dx%d\n", s->aqua.fishes[tabfish[i]].name, trad_coorx(&s->aqua,client->id_view,s->aqua.fishes[tabfish[i]].pos.y),trad_coory(&s->aqua,client->id_view,s->aqua.fishes[tabfish[i]].pos.y), s->aqua.fishes[tabfish[i]].size.width,  s->aqua.fishes[tabfish[i]].size.height);
       send(client->socket,msg2,strlen(msg2),0);	      
    }
	
    free(tabfish);
}

  return 0;
}



