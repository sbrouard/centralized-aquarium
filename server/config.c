#include "config.c"

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
	int file,length,status,value;
	char c,value_name[MAX_LEN_VALUE_NAME];

	default_config(sc);

	file = open(CONFIG_FILE_NAME,O_RDONLY);

	if(file == -1)
	{
		perror("open()");
		return ;
	}

	printf("parsing config file...\n");

	length = 0;
	status = 0;
	value = 0;

	while(read(file,&c,1) == 1)
	{
		switch(status)
		{
			case 0:
				if(c != ' ' && c != '=')
				{
					if(length > MAX_LEN_VALUE_NAME)
					{
						printf("Unknow value name\n");
						status = 5;
					}
					value_name[length] = c;
					length++;
					break;
				}	
			case 1:
				status = 1;
				if(c == ' ')
					break;
				else if(c != '=')
				{
					printf("Unexpected %c\n",c);
					status = 5;
					break;
				}
			case 2 :
				status = 2;
				if(c == ' ')
					break;
				else if(c < '0' || c > '9')
				{
					printf("Unexpected %c\n",c);
					status = 5;
					break;
				}
			case 3 :
				status = 3;

				if(c >= '0' && c <= '9')
				{
					value += c - 0x31;
					break;
				}else if(c != ' ' && c != '\n')
				{
					printf("Unexpected %c\n",c);
					status = 5;
					break;
				}
			case 4 :
				status = 4;
				if(c == ' ')
					break;
				else if(c == '\n')
				{
					for(i=0;i<NBR_VALUE_NAME;i++)
					{
						if(strncmp(value_name,name_fctns[i].value_name,length) == 0)
						{
							name_fctn[i].fonction(sc,value);
							break;
						}
					}

					if(i==NBR_VALUE_NAME)
					{
						printf("Unknow value name\n");
					}
					length = 0;
					status = 0;
					value = 0;

					break;
				}else
				{
					printf("Unexpected %c\n",c);
					status = 5;
					break;

				}
				break;
			case 5:
				if(c == '\n')
				{
					length = 0;
					status = 0;
					value = 0;
				}
				break;
		}

	if(status == 4)
	{

		for(i=0;i<NBR_VALUE_NAME;i++)
		{
			if(strncmp(value_name,name_fctns[i].value_name,length) == 0)
			{
				name_fctn[i].fonction(sc,value);
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
	sd->controller_port = value;
}

void fill_display_timeout_value(server_config* sc,int value);
{
	sd->display_timeout = value;
}

void fill_fish_update_interval(server_config* sc,int value);
{
	sd->fish_update_interval = value;
}

void fill_max_client(server_config* sc,int value);
{
	sd->max_client = value;
}
