#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SIZE_CMD 6
#define NO_MORE_READ 256
#define DEFAULT_STR_SIZE 32
//#define READ_ERROR 257

typedef struct str_arg{
	char *str;
	int length;
	int malloc_size;
}str_arg;

typedef struct command_load{
	str_arg filename;
}command_load;

typedef struct terminal{
	int state;
	uint8_t is_killed;
	int command_length;
	char command[MAX_SIZE_CMD];
	int is_next_get;
	unsigned char next_c;
	int caracter_count;
	int (*function)(struct terminal*);

	command_load cl;
}terminal;

typedef struct command_function{
	char command[MAX_SIZE_CMD];
	int (*function)(terminal*);
	//ptr sur fonction de parsage
}command_function;

void read_terminal(terminal *);
int parse_load(terminal*);
int parse_exit(terminal*);

void cmd_load(terminal*);

#endif
