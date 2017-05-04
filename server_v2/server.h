#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/select.h>
#include <time.h>

#include "config.h"


#define MAX_VIEWS 9
#define MAX_SIZE_COMMAND 256

#define AVAILABLE -1
#define NOTAVAILABLE 1
#define NOVIEW -1

#define UNKNOWN_COMMAND 1

#define MAX_FISHES 1024
#define NAME_LENGTH 256

//DUREE DEPLACEMENT DES POISSONS
#define DEFAULT_DURATION 5

//MOVE_FISHES
#define INTERVAL_UPDATE_FISHES 10


#define CALCULATED 1
#define NOT_CALCULATED 0

#define ORIGIN 0
#define DEST 1
#define NOT_MOVING 2
#define NOT_IN_VIEW 3

#define YES 1
#define NO 0

struct coord{
  int x;
  int y;
};

struct dimensions{
  int width;
  int height;
};  


struct view{
  int client;
  struct coord pos;
  struct dimensions size;
};

struct fish{
  char name[NAME_LENGTH];
  struct dimensions size;
  char mobility[NAME_LENGTH];
  struct coord pos;
  struct coord old_pos;
  int isStarted;

  struct coord postosend[MAX_VIEWS];
  
  struct coord origin[MAX_VIEWS];
  struct coord dest[MAX_VIEWS];
  int originiscalculated[MAX_VIEWS];
  int destiscalculated[MAX_VIEWS];
  int progress[MAX_VIEWS];
  int traverseorigin[MAX_VIEWS];
  int traversedest[MAX_VIEWS];
  
};

struct aquarium{
  struct dimensions size;
  struct view* views;
  int nb_views,nb_views_allocated;
  struct fish fishes[MAX_FISHES];
  int nb_fishes;
};



struct client_data{
  int socket;
  int id_view;
  char buffer[MAX_SIZE_COMMAND];
  int buffer_size;
  int update_continuously;
  struct timeval give_continuously;
  struct timeval last_activity;
};

struct server{
  int socket;
  struct client_data *client_list;
  int nb_client;
  struct aquarium aqua;
  struct server_config conf;
  struct timeval update_fishes;
};


int fd_to_read(struct server *,fd_set *,struct timeval *timeout);
int read_server(struct server *,fd_set *);
int init_server(struct server *);

int parse(struct client_data*, struct server*);
int hello(struct client_data*, int indice, struct server*);
int getFishes(struct client_data*, int indice, struct server *);
int getFishesContinuously(struct client_data*, int indice, struct server *);
int log_out(struct client_data*, int indice, struct server *);
int ping(struct client_data*, int indice);
int status(struct client_data*, int indice, struct server *);
int addFish(struct client_data*, int indice, struct server *);
int delFish(struct client_data*, int indice, struct server *);
int startFish(struct client_data*, int indice, struct server *);
int read_client(struct client_data*, struct server *);
int sendFishesOfView(struct client_data*,struct server *);


int moveFishes(struct server *);
int moveFish(struct fish *, struct server *);




#endif











