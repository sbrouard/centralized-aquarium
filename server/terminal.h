#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"

#define MAX_SIZE_CMD 6
#define NO_MORE_READ 256
#define DEFAULT_STR_SIZE 32
//#define READ_ERROR 257

typedef struct str_arg{
	char *str;
	int length;
	int malloc_size;
}str_arg;

typedef struct command_view{
	str_arg id;
	int pos_x,pos_y;
	int width,height;
}command_view;

typedef struct terminal{
	int state;
	uint8_t is_killed;
	int command_length;
	char command[MAX_SIZE_CMD];
	int is_next_get;
	unsigned char next_c;
	int caracter_count;
	int (*function)(struct terminal*);
	struct server serv;

	str_arg split_cmd;
	command_view cv;
}terminal;

typedef struct command_function{
	char command[MAX_SIZE_CMD];
	int (*function)(terminal*);
	//ptr sur fonction de parsage
}command_function;

void zero_str_arg(str_arg *str);
void free_str_arg(str_arg *str);
void read_terminal(terminal *);
int parse_load(terminal*);
int parse_exit(terminal*);
int parse_save(terminal *term);
int parse_show_aquarium(terminal *term);
int parse_add_view(terminal *term);
int parse_del_view(terminal *term);
int parse_launch(terminal *term);

void cmd_load(terminal*);
void cmd_save(terminal *term);
void cmd_show_aquarium(terminal *term);
void cmd_add_view(terminal *term);
void cmd_del_view(terminal *term);
void cmd_launch(terminal *term);

#endif
