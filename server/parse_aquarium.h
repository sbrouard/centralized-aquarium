#ifndef PARSE_AQUARIUM
#define PARSE_AQUARIUM

#include <stdio.h>
#include <stdlib.h>
#include "server.h"

#define NO_CHAR 256
#define END_OF_FILE 257
#define START_ALLOC_NB_VIEW 10

typedef struct cursor{
	int file;
	int next_char;
}cursor;

int parse_aquarium(char *filename,struct aquarium *aq);

#endif
