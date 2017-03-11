#include "config.h"

#define MAX_LEN_VALUE_NAME 21
#define NBR_VALUE_NAME 4
value_name_fonction name_fctns[] = {{"controller-port",fill_controller_port},
				{"display-timeout-value",fill_display_timeout_value},
				{"fish-update-interval",fill_fish_update_interval},
				{"max-client",fill_max_client}};

void default_config(server_config *sc)
{
	sc->controller_port = DEFAULT_CONTROLLER_PORT;
	sc->display_timeout_value = DEFAULT_DISPLAY_TIMEOUT;
	sc->fish_update_interval = DEFAULT_FISH_UPDATE_INTERVAL;
	sc->max_client = DEFAULT_MAX_CLIENT;
}

void parse_config_file(server_config *sc)
{
	int file,length,status,value,i,line,c_number;
	char c,value_name[MAX_LEN_VALUE_NAME];

	default_config(sc);

	file = open(CONFIG_FILE_NAME,O_RDONLY);

	if(file == -1)
	{
		printf("failed to open config file :\n");
		perror("open()");
		return ;
	}

	printf("parsing config file...\n");

	length = 0;
	status = 0;
	value = 0;
	line = 0;
	c_number = 0;

	while(read(file,&c,1) == 1)
	{
		c_number++;
		switch(status)
		{
			case 0:
				if(c == ' ' || c =='\t')
					break;
				else if( c == '\n')
				{
					line++;
					break;
				}
				else if(c == '#')
				{
					status = 7;
					break;
				}
			case 1:
				if(c != ' '&& c != '\t' && c != '=')
				{
					if(length > MAX_LEN_VALUE_NAME)
					{
						printf("line %d:%d Unknow value name\n",line,c_number);
						status = 7;
					}
					value_name[length] = c;
					length++;
					break;
				}	
			case 2:
				status = 2;
				if(c == ' ' || c == '\t')
					break;
				else if(c == '=')
				{
					status = 3;
					break;
				}else
				{
					printf("line %d:%d Unexpected %c\n",line,c_number,c);
					status = 7;
					break;
				}
			case 3:
				status = 3;
				if(c == ' ' || c =='\t')
					break;
			case 4 :
				status = 4;
				if(c < '0' || c > '9')
				{
					printf("line %d:%d Unexpected %c\n",line,c_number,c);
					status = 7;
					break;
				}
			case 5 :
				status = 5;

				if(c >= '0' && c <= '9')
				{
					value = value*10 + c - 0x30;
					break;
				}else if(c != ' ' && c != '\n' && c != '\t')
				{
					printf("line %d:%d Unexpected %c\n",line,c_number,c);
					status = 7;
					break;
				}
			case 6 :
				status = 6;
				if(c == ' ' || c == '\t')
					break;
				else if(c == '\n')
				{
					for(i=0;i<NBR_VALUE_NAME;i++)
					{
						if(strncmp(value_name,name_fctns[i].value_name,length) == 0)
						{
							name_fctns[i].fonction(sc,value);
							break;
						}
					}

					if(i==NBR_VALUE_NAME)
					{
						printf("line %d:%d Unknow value name\n",line,c_number);
					}
					length = 0;
					status = 0;
					value = 0;
					c_number = 0;
					line++;

					break;
				}else
				{
					printf("line %d:%d Unexpected %c\n",line,c_number,c);
					status = 7;
					break;

				}
				break;
			case 7:
				if(c == '\n')
				{
					line++;
					length = 0;
					status = 0;
					value = 0;
					c_number = 0;
				}
				break;
		}
	}

	if(status == 4)
	{

		for(i=0;i<NBR_VALUE_NAME;i++)
		{
			if(strncmp(value_name,name_fctns[i].value_name,length) == 0)
			{
				name_fctns[i].fonction(sc,value);
				break;
			}
		}

		if(i==NBR_VALUE_NAME)
		{
			printf("Unknow value name\n");
		}
	}

	close(file);

}

void fill_controller_port(server_config* sc,int value)
{
	printf("parsed controller_port : %d\n",value);
	sc->controller_port = value;
}

void fill_display_timeout_value(server_config* sc,int value)
{
	printf("parsed display_timeout_value : %d\n",value);
	sc->display_timeout_value = value;
}

void fill_fish_update_interval(server_config* sc,int value)
{
	printf("parsed fish_update_interval : %d\n",value);
	sc->fish_update_interval = value;
}

void fill_max_client(server_config* sc,int value)
{
	printf("parsed max_client : %d\n",value);
	sc->max_client = value;
}
