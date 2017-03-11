#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEFAULT_CONTROLLER_PORT 12345
#define DEFAULT_DISPLAY_TIMEOUT 45
#define DEFAULT_FISH_UPDATE_INTERVAL 1
#define DEFAULT_MAX_CLIENT 20
#define CONFIG_FILE_NAME "controller.cfg"

typedef struct server_config{
	int controller_port;
	int display_timeout_value;
	int fish_update_interval;
	int max_client;
}server_config;

typedef struct value_name_fonction{
	char *value_name;
	void (*fonction)(server_config*,int);
}value_name_fonction;

void parse_config_file(server_config*);

void fill_controller_port(server_config*,int);
void fill_display_timeout_value(server_config*,int);
void fill_fish_update_interval(server_config*,int);
void fill_max_client(server_config*,int);
#endif
