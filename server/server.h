#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "terminal.h"


#define MAX_VIEWS 8
#define MAX_SIZE_COMMAND 256




struct coord{
  int x;
  int y;
};

struct dimensions{
  int width;
  int height;
};  


struct view{
  struct coord pos;
  struct dimensions size;

};

struct aquarium{
  struct dimensions size;
  struct view* views;
  struct fish* fishes;
};

struct fish{
  char * name;
  struct dimensions size;
  char * mobility;
  struct coord pos;
};

struct controller{
  int port;
  int timeout;
  int update_time;
};

struct client_data{
  int socket;
  char buffer[MAX_SIZE_COMMAND];
  int buffer_size;
};

typedef struct server{
	int socket;
	struct client_data *client_list;
	int nb_client;
	terminal term;
}server;

int read_server(server *,fd_set*);
int fd_to_read(server *,fd_set *);
int init_server(server *);

int initialization(int);

int parse(struct client_data*);
int hello(struct client_data*, int indice);
int getFishes(struct client_data*, int indice);
int getFishesContinously(struct client_data*, int indice);
int log_out(struct client_data*, int indice);
int ping(struct client_data*, int indice);
int status(struct client_data*, int indice);
int addFish(struct client_data*, int indice);
int delFish(struct client_data*, int indice);
int startFish(struct client_data*, int indice);
int read_client(struct client_data*);






#endif











