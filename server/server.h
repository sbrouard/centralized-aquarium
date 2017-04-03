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

#include "config.h"


#define MAX_VIEWS 9
#define MAX_SIZE_COMMAND 256

#define AVAILABLE -1
#define NOTAVAILABLE 1
#define NOVIEW -1

#define UNKNOWN_COMMAND 1

#define MAX_FISHES 1024
#define NAME_LENGTH 256

#define DEFAULT_DURATION 5


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
};

struct server{
  int socket;
  struct client_data *client_list;
  int nb_client;
  struct aquarium aqua;
  struct server_config conf;
};


int fd_to_read(struct server *,fd_set *);
int read_server(struct server *,fd_set *);
int init_server(struct server *);

int initialization(int);

int parse(struct client_data*, struct server*);
int hello(struct client_data*, int indice, struct server*);
int getFishes(struct client_data*, int indice, struct server *);
int getFishesContinously(struct client_data*, int indice, struct server *);
int log_out(struct client_data*, int indice, struct server *);
int ping(struct client_data*, int indice);
int status(struct client_data*, int indice, struct server *);
int addFish(struct client_data*, int indice, struct server *);
int delFish(struct client_data*, int indice, struct server *);
int startFish(struct client_data*, int indice, struct server *);
int read_client(struct client_data*, struct server *);






#endif











