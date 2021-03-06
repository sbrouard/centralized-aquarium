#include "server.h"

#define TEST {printf("test\n");fflush(0);}

int init_client(struct client_data *c, int sock, struct server *s)
{
  c->socket = sock;
  c->id_view = -1;//PAS ENCORE PRIS EN COMPTE
  c->buffer_size = 0;
  c->update_continuously = 0;
  c->last_activity.tv_sec = s->conf.display_timeout_value;
  c->last_activity.tv_usec = 0;
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

  else if (strncmp(client->buffer,"getFishesContinuously",i) == 0){
    getFishesContinuously(client,i,s);
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

	shutdown(client->socket,SHUT_WR|SHUT_RD);
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

	client->last_activity.tv_sec = s->conf.display_timeout_value;
 	client->last_activity.tv_usec = 0;

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
	t->update_fishes.tv_sec = INTERVAL_UPDATE_FISHES;
	t->update_fishes.tv_usec = 0;

	return 0;
}

int fd_to_read(struct server *serv,fd_set *set,struct timeval *timeout)
{
	int nfds = 0,i;

	if(serv->socket != -1)
	{
		nfds = serv->socket;
		FD_SET(serv->socket,set);
	}

	timeout->tv_sec = serv->update_fishes.tv_sec;
	timeout->tv_usec = serv->update_fishes.tv_usec;

	for(i=0;i<serv->nb_client;i++)
	{
		FD_SET(serv->client_list[i].socket,set);

		if(serv->client_list[i].socket > nfds)
			nfds = serv->client_list[i].socket;

		if(serv->client_list[i].update_continuously)
		{
			if(serv->client_list[i].give_continuously.tv_sec < timeout->tv_sec)
			{
				timeout->tv_sec = serv->client_list[i].give_continuously.tv_sec;
				timeout->tv_usec = serv->client_list[i].give_continuously.tv_usec;
			}else if(serv->client_list[i].give_continuously.tv_sec == timeout->tv_sec)
			{
				if(timeout->tv_usec > serv->client_list[i].give_continuously.tv_usec)
				{
					timeout->tv_usec = serv->client_list[i].give_continuously.tv_usec;
				}

			}
		}

		if(serv->client_list[i].last_activity.tv_sec < timeout->tv_sec)
 		{
 			timeout->tv_sec = serv->client_list[i].last_activity.tv_sec;
 			timeout->tv_usec = serv->client_list[i].last_activity.tv_usec;
 		}else if(serv->client_list[i].last_activity.tv_sec == timeout->tv_sec)
 		{
 			if(timeout->tv_usec > serv->client_list[i].last_activity.tv_usec)
 			{
 				timeout->tv_usec = serv->client_list[i].last_activity.tv_usec;
 			}
 
 		}
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
		init_client(&serv->client_list[serv->nb_client],accept(serv->socket,(struct sockaddr*)&client_in,&size),serv);
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

//Traduit une abscisse locale en % en abscisse globale  
int trad_inv_coorx(struct aquarium *a,int view,int x){
  int newx = x;

  newx = (newx*a->views[view].size.width)/100 + a->views[view].pos.x;  

  return newx;
 }

//Traduit une ordonnee locale en % en ordonnee globale 
int trad_inv_coory(struct aquarium *a,int view, int y){
  int newy = y;

  newy = (newy*a->views[view].size.height)/100 + a->views[view].pos.y;  

  return newy;
 }



//Traduit une abscisse globale en abscisse locale % 
int trad_coorx(struct aquarium *a,int view,int x){
  int newx = x;
  newx = newx - a->views[view].pos.x;
   

  newx = (newx*100)/(a->views[view].size.width);
   
  return newx;
 }

//Traduit une ordonnee globale en ordonnee locale % 
int trad_coory(struct aquarium *a,int view, int y){
  int newy = y;

  newy = newy - a->views[view].pos.y;
		    


  newy = (newy*100)/(a->views[view].size.height);
		 
  return newy;
 }






//utilisee dans hello() pour factoriser l'affectation d'une vue a un client
int affect_available_view(struct server *s, struct client_data* client){
  int j;
  char greet[512];
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





int fishIsInView(struct fish* f, struct view* v){
  if (f->pos.x >= v->pos.x && f->pos.x <= v->pos.x + v->size.width && f->pos.y >= v->pos.y && f->pos.y <= v->pos.y + v->size.height)
    return 1;
  else 
    return 0;
}

int findFishesOfView(struct aquarium* a, int view, int *tabfish)
{
  int len = 0;
  for (int i = 0; i < a->nb_fishes; ++i)
    {
      if (a->fishes[i].progress[view] != NOT_IN_VIEW)
	{
	  if (a->fishes[i].progress[view] == ORIGIN)
	    {
	      ++len;
	      tabfish = realloc(tabfish,len);
	      a->fishes[i].progress[view] = DEST;

	      struct coord posorigin = a->fishes[i].origin[view];
	      if (a->fishes[i].traverseorigin[view] == YES)
		{
		  if (trad_coorx(a,view,posorigin.x) == 0)
		    {
		      posorigin.x -= a->fishes[i].size.width * a->views[view].size.width / 100;
		    }
		  if (trad_coory(a,view,posorigin.y)== 0)
		    {
		      posorigin.y -= a->fishes[i].size.height * a->views[view].size.height / 100;
		    }
		}

	      a->fishes[i].postosend[view] = posorigin;
	      tabfish[len-1] = i; 
	    }
	  else if (a->fishes[i].progress[view] == DEST)
	    {
	      ++len;
	      tabfish = realloc(tabfish,len);
	      if (fishIsInView(&(a->fishes[i]), &(a->views[view])))
		  a->fishes[i].progress[view] = NOT_MOVING;
	      else a->fishes[i].progress[view] = NOT_IN_VIEW;
	       
	      struct coord posdest = a->fishes[i].dest[view];
	      if (a->fishes[i].traversedest[view] == YES)
		{
		  if (trad_coorx(a,view,posdest.x) == 0)
		    {
		      posdest.x -= a->fishes[i].size.width * a->views[view].size.width / 100;
		    }
		  if (trad_coory(a,view,posdest.y) == 0)
		    {
		      posdest.y -= a->fishes[i].size.height * a->views[view].size.height / 100;
		    }
		}

	      a->fishes[i].postosend[view] = posdest;
	      tabfish[len-1] = i;
	    }
	  else if (a->fishes[i].progress[view] == NOT_MOVING)
	    {
	      ++len;
	      tabfish = realloc(tabfish,len);
	      a->fishes[i].progress[view] = NOT_MOVING;
	      a->fishes[i].postosend[view] = a->fishes[i].dest[view];
	      tabfish[len-1] = i;

	    }
	}
    }

  return len;
 
}


//auxiliaire pour getFishes et getFishesContinuously qui envoie la liste des poissons de la vue
int sendFishesOfView(struct client_data* client, struct server* s){
  char msg[4096];
  int ajout = 0;
  int * tabfish = malloc(sizeof(int));
  int len = findFishesOfView(&(s->aqua), client->id_view, tabfish);
  if (len > 0){
    sprintf(msg, "list");
    for (int i = 0; i < len; i++)
      {
	ajout += sprintf(msg+4+ajout, " [%s at %dx%d,%dx%d,%d]",
			 s->aqua.fishes[tabfish[i]].name,
			 trad_coorx(&s->aqua,client->id_view,s->aqua.fishes[tabfish[i]].postosend[client->id_view].x),
			 trad_coory(&s->aqua,client->id_view,s->aqua.fishes[tabfish[i]].postosend[client->id_view].y), 
			 s->aqua.fishes[tabfish[i]].size.width,  s->aqua.fishes[tabfish[i]].size.height, (s->aqua.fishes[tabfish[i]].progress[client->id_view] == NOT_MOVING) ? 2*DEFAULT_DURATION : DEFAULT_DURATION);
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


int getFishesContinuously(struct client_data* client, int indice, struct server * s){
   if (client->buffer[indice] != '\n'){
     char * unknown = "NOK : commande introuvable\n";
     send(client->socket,unknown, strlen(unknown),0);
   }
   else {
     client->update_continuously = 1;
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


//retourne l'indice dans le tableau des poissons si le nom du poisson est déjà pris, -1 sinon
int alreadyExistsFish(struct server *s, const char * name){
  
  int i;
  for (i = 0; i < s->aqua.nb_fishes; ++i){
    if (strcmp(name, s->aqua.fishes[i].name) == 0){
      return i;
    }

  }

  return -1;
    

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
        f.name[name_length] = '\0';
      int exist = alreadyExistsFish(s,f.name);
      if (exist != -1){ // Cas nom poisson deja existant
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
    for ( ;client->buffer[i] != ' ' && i < client->buffer_size; ++i){
      if (client->buffer[i] != 'x' && j < 3 && nb == 1){
	msg[j] = client->buffer[i];
	++j;
      }
      else if (client->buffer[i] == 'x' && j != 0 && nb == 1){
	pos = atoi(msg);
	++nb;
	if ( pos >= 0 && pos <= 100){
	  f.pos.x = trad_inv_coorx(&s->aqua,client->id_view,pos); //We place the fish in the client view
	  f.old_pos.x = f.pos.x;
	  printf("posx %d\n", f.pos.x);
	  j = 0;
	  for (int c = 0; c < 3; ++c){
	    msg[c] = '\0';
	  }
	}
      }

      else if (nb == 2 && j < 3 && client->buffer[i] != ','){
	msg[j] = client->buffer[i];
	++j;
      }

      else if (nb == 2 && client->buffer[i] == ',' && j != 0){
	pos = atoi(msg);
	if ( pos >= 0 && pos <= 100){
	  f.pos.y = trad_inv_coory(&s->aqua,client->id_view,pos);
	  f.old_pos.y = f.pos.y;
	  printf("posy %d\n", f.pos.y);
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
    ++i;
    char msg2[3] = { '\0', '\0', '\0'};
    for ( ;client->buffer[i] != ' ' && i < client->buffer_size; ++i){
      if (client->buffer[i] != 'x' && j < 3 && nb == 1){
	msg2[j] = client->buffer[i];
	++j;
      }
      else if (client->buffer[i] == 'x' && j != 0 && nb == 1){
	pos = atoi(msg2);
	++nb;
	if ( pos >= 0 && pos <= 100){
	  f.size.width = pos;
	  printf("sizewidth %d\n", f.size.width);
	  j = 0;
	  for (int c = 0; c < 3; ++c){
	    msg2[c] = '\0';
	  }
	}
      }

      else if (nb == 2 && j < 3 && client->buffer[i] != ','){
	msg2[j] = client->buffer[i];
	++j;
      }

      else if (nb == 2 && client->buffer[i] == ',' && j != 0){
	pos = atoi(msg2);
	if ( pos >= 0 && pos <= 100){
	  f.size.height = pos;
	  printf("sizeheight %d\n", f.size.height);
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
    if ( client->buffer[i] != ' '){
      	send(client->socket,unknown, strlen(unknown),0);
	return UNKNOWN_COMMAND;
    }
    
    
    char mobility[NAME_LENGTH];
    j = 0;
    
    for (i+=1; client->buffer[i] != '\n' && j < NAME_LENGTH; ++i){
      mobility[j] = client->buffer[i];
      ++j;
    }
    
    if (j >= NAME_LENGTH){
      send(client->socket,unknown, strlen(unknown),0);
      return UNKNOWN_COMMAND;	
    }
      
    strcpy(f.mobility, mobility);
    printf("mobi %s\n", f.mobility);
    f.isStarted = 0;

    
    memset(f.progress, NOT_IN_VIEW, MAX_VIEWS*sizeof(int));
    memset(f.originiscalculated, NOT_CALCULATED, MAX_VIEWS*sizeof(int));
    memset(f.destiscalculated, NOT_CALCULATED, MAX_VIEWS*sizeof(int));
    memset(f.traverseorigin, NO, MAX_VIEWS*sizeof(int));
    memset(f.traversedest, NO, MAX_VIEWS*sizeof(int));

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
      f.name[name_length] = '\0';
      int exist = alreadyExistsFish(s,f.name);
      if (exist == -1){ // Cas poisson inexistant
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
      f.name[name_length] = '\0';
      int exist = alreadyExistsFish(s,f.name);
      if (exist == -1){ // Cas poisson inexistant
	send(client->socket, "NOK : Poisson inexistant\n", 25,0);
	return UNKNOWN_COMMAND;
      }
      else{
	s->aqua.fishes[exist].isStarted = 1;
	printf("%s started\n",f.name);

      }
    }
    else {
      send(client->socket, "NOK\n", 4,0);
	return UNKNOWN_COMMAND;
    }
  }
  return 0;
}

//remplit par effet de bord un tableau avec les indices des poissons de la vue (pour la commande status)
int getFishesInView(struct aquarium* a, int view, int *tabfish ){
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
    int len = getFishesInView(&(s->aqua), client->id_view, tabfish);
    sprintf(msg1, "OK : Connecte au controleur, %d poisson(s) trouve(s)\n", len);
    send(client->socket,msg1, strlen(msg1),0);
    for (i = 0 ; i < len; ++i){
      sprintf(msg2, "Fish %s at %dx%d,%dx%d\n", 
	      s->aqua.fishes[tabfish[i]].name,
	      trad_coorx(&s->aqua,client->id_view,s->aqua.fishes[tabfish[i]].pos.x),
	      trad_coory(&s->aqua,client->id_view,s->aqua.fishes[tabfish[i]].pos.y),
	      s->aqua.fishes[tabfish[i]].size.width,  
	      s->aqua.fishes[tabfish[i]].size.height);

       send(client->socket,msg2,strlen(msg2),0);	      
    }
	
    free(tabfish);
}
    
  return 0;
}


struct coord RandomWayPoint(struct server *s)
{
  int x = rand() % (s->aqua.size.width);
  int y = rand() % (s->aqua.size.height);
  struct coord newpos;
  newpos.x = x;
  newpos.y = y;
  printf("%d %d\n",x,y);
  return newpos;
}

//verifie si une coordonnee est dans une vue
int coordIsInView(struct coord pos, struct view* v)
{
  if (pos.x >= v->pos.x && pos.x <= v->pos.x + v->size.width && pos.y >= v->pos.y && pos.y <= v->pos.y + v->size.height)
    return 1;
  else 
    return 0;
}


int checkViews(struct coord pos, struct server *s, struct fish *f)
{
  for (int i = 0; i < s->aqua.nb_views; ++i)
    {
      //     printf("or %d de %d\n",f->originiscalculated[2],f->destiscalculated[2]);
      if (f->originiscalculated[i] == NOT_CALCULATED)
	{
	  if (coordIsInView(pos,&(s->aqua.views[i])))
	    {
	      f->origin[i] = pos;
	      printf("origin vue %d : %d %d\n",i,pos.x,pos.y);
	      f->originiscalculated[i] = CALCULATED;
	      f->progress[i] = ORIGIN;
	      f->traverseorigin[i] = YES;
	    }
	}
      else
	{
	  if (f->destiscalculated[i] == NOT_CALCULATED)
	    {
	      if (!coordIsInView(pos,&(s->aqua.views[i])))
		{
		  f->dest[i] = pos;
		  printf("dest vue %d : %d %d\n", i, pos.x,pos.y);
		  f->destiscalculated[i] = CALCULATED;  
		  f->traversedest[i] = YES;
		}	
	    }
	}
	
    }
  return 0;

}
	
	
	
	  
int moveFishes(struct server *s)
{
  for (int i = 0; i < s->aqua.nb_fishes; ++i){
    if (s->aqua.fishes[i].isStarted){
      moveFish(&s->aqua.fishes[i],s);
      

      memset(s->aqua.fishes[i].originiscalculated, NOT_CALCULATED, MAX_VIEWS*sizeof(int));
      memset(s->aqua.fishes[i].destiscalculated, NOT_CALCULATED, MAX_VIEWS*sizeof(int));
      memset(s->aqua.fishes[i].traverseorigin, NO, MAX_VIEWS*sizeof(int));
    memset(s->aqua.fishes[i].traversedest, NO, MAX_VIEWS*sizeof(int));

      struct coord init = s->aqua.fishes[i].old_pos;
      struct coord end = s->aqua.fishes[i].pos;

      for (int v = 0; v < s->aqua.nb_views; ++v)
	{
	  if (coordIsInView(end,&(s->aqua.views[v])))
	    {
	      s->aqua.fishes[i].destiscalculated[v] = CALCULATED;
	      s->aqua.fishes[i].dest[v] = end;
	      printf("dest vue %d : %d %d\n",v, end.x,end.y);
	      if (coordIsInView(init,&(s->aqua.views[v])))
		{
		  s->aqua.fishes[i].originiscalculated[v] = CALCULATED;
		  s->aqua.fishes[i].origin[v] = init;
		  printf("origin vue %d : %d %d\n",v, init.x,init.y);
		  s->aqua.fishes[i].progress[v] = DEST;
		}
	    }
	  else if (coordIsInView(init,&(s->aqua.views[v])))
		{
		  s->aqua.fishes[i].originiscalculated[v] = CALCULATED;
		  s->aqua.fishes[i].origin[v] = init;
		  printf("origin vue %d : %d %d\n",v, init.x,init.y);
		  s->aqua.fishes[i].progress[v] = DEST;
		}

	}

      
      int count = -1;
      int distance = 0;
      while (init.x != end.x || init.y != end.y)
	{
	  ++count;
	  if (count % 2 == 0)
	    {
	      if (init.x < end.x)
		{
		  checkViews(init,s,&(s->aqua.fishes[i]));
		  ++init.x;
		  ++distance;
		}
	      else if (init.x > end.x)
		{
		  checkViews(init,s,&(s->aqua.fishes[i]));
		  --init.x;
		  ++distance;
		}
	      
	    }
	  else
	    {
	      if (init.y < end.y)
		{
		  checkViews(init,s,&(s->aqua.fishes[i]));
		  ++init.y;
		  ++distance;
		}
	      else if (init.y > end.y)
		{
		  checkViews(init,s,&(s->aqua.fishes[i]));
		  --init.y;
		  ++distance;
		}
	    }
	}
      printf("distance : %d\n",distance);
 
    }

  }

  return 0;
}


int moveFish(struct fish *f, struct server *s)
{
  if (strcmp(f->mobility,"RandomWayPoint") == 0){
    struct coord newpos = RandomWayPoint(s);
    f->old_pos.x = f->pos.x;
    f->old_pos.y = f->pos.y;
    f->pos.x = newpos.x;
    f->pos.y = newpos.y;
   
    return 0;
  }
  else {
    printf("Unknown mobility %s\n", f->mobility);
    return -1;
  }

}
