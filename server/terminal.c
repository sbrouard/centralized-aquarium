#include "terminal.h"

//ajouter des ' ' pour les commande pour remplire les MAX_SIZE_CMD caractéres
#define NBR_CMD 7
command_function cmd_func[]={{"load  ",parse_load},
				{"exit  ",parse_exit},
				{"save  ",parse_save},
				{"show  ",parse_show_aquarium},
				{"add   ",parse_add_view},
				{"del   ",parse_del_view},
				{"launch",parse_launch}};

int init_terminal(terminal *term)
{
	term->command_length = 0;
	term->state = 0;
	term->is_next_get = 0;
	term->caracter_count = 0;
	term->is_killed = 0;

	fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

	zero_str_arg(&term->cv.id);
	zero_str_arg(&term->split_cmd);
	term->cv.pos_x = -1;
	term->cv.pos_y = -1;
	term->cv.width = -1;
	term->cv.height = -1;
}

void zero_str_arg(str_arg *str)
{
	str->length = 0;
	str->malloc_size = 0;
	str->str = NULL;
}

void free_str_arg(str_arg *str)
{
	free(str->str);
	zero_str_arg(str);
}

int next_char(terminal *term)
{
	int res;
	if(!term->is_next_get)
	{
		res = read(0,&term->next_c,1);
		if(res == -1 || res == 0)
		{
			if(errno != EWOULDBLOCK && errno != EAGAIN)
			{
				perror("read()");
				exit(1);
			}else
			{
				return NO_MORE_READ;
			}
		}
		term->is_next_get = 1;
	}
		
	return term->next_c;
}

int get_char(terminal *term)
{
	unsigned int c;

	c = next_char(term);
	term->is_next_get = 0;
	term->caracter_count++;

	return c;
}

int flush_cmd(terminal *term)
{
	for(;;)
	{
		switch(get_char(term))
		{
			case '\n':
				return 1;
			case NO_MORE_READ:
				return 0;
			default:
				break;
		}
	}
}

int syntax_error(char *error, terminal *term)
{
	int i;

	for(i=0;i<term->caracter_count;i++)
		printf(" ");
	printf("^\n");
	printf("Syntax error : %s\n",error);
	term->function = flush_cmd;
	term->cv.pos_x = -1;
	term->cv.pos_y = -1;
	term->cv.width = -1;
	term->cv.height = -1;
	return flush_cmd(term);
}

void read_terminal(terminal *term)
{
	int i;

	if(term->state != 0)
	{
		if((term->function)(term))
		{
			term->state = 0;
			term->command_length = 0;
			term->caracter_count = 0;
		}else
			return;
	}

	while(next_char(term) != NO_MORE_READ)
	{
		if(next_char(term) == ' ' || next_char(term) == '\n')
		{
			if(term->command_length > 0)
			{
				i=0;
				while(i < NBR_CMD && strncmp(term->command,cmd_func[i].command,term->command_length) != 0)
					i++;

				if(i == NBR_CMD)
				{
					if(syntax_error("Unknow command",term))
					{
						term->state = 0;
						term->command_length = 0;
						term->caracter_count = 0;
					}else
						return;
				}else
				{
					term->state = 1;
					term->function = cmd_func[i].function;
					if((cmd_func[i].function)(term))
					{
						term->state = 0;
						term->command_length = 0;
						term->caracter_count = 0;
					}else
						return;
				}
			}else
			{
				get_char(term);
				term->caracter_count = 0;
			}
		}else
		{
			term->command[term->command_length] = get_char(term);
			++term->command_length;
		}
		if(term->command_length > MAX_SIZE_CMD)
		{
			if(syntax_error("Unknow command",term))
			{
				term->state = 0;
				term->command_length = 0;
				term->caracter_count = 0;
			}else
				return;
		}
	}

	return;
}

//common parsing functions
int parse_blank(terminal *term)
{
	while(next_char(term) == ' ')
	{
		get_char(term);
	}

	if(next_char(term) == NO_MORE_READ)
		return 0;
	else
		return 1;
}

int parse_str_arg(terminal *term,str_arg *str)
{
	if(str->malloc_size == 0)
	{
		str->str = malloc(DEFAULT_STR_SIZE);
		str->malloc_size = DEFAULT_STR_SIZE;
	}

	for(;;)
	{
		switch(next_char(term))
		{
			case ' ':
			case '\n':
				return 1;
			case NO_MORE_READ:
				return 0;
			default:
				if(str->length == str->malloc_size)
				{
					str->malloc_size *= 2;
					str->str = realloc(str->str,str->malloc_size);
				}
				str->str[str->length] = get_char(term);
				str->length++;
		}
	}
}

int parse_int(terminal *term,int *value)
{
	for(;;)
	{
		if(next_char(term) >= '0' && next_char(term) <= '9')
		{
			if(*value == -1)
				*value = 0;
			*value = (*value)*10 + get_char(term)-0x30;
		}else if(next_char(term) == NO_MORE_READ)
			return 0;
		else
			return 1;
	}
}
//command parsing functions
int parse_launch(terminal *term)
{
	switch(term->state)
	{
		case 1:
			term->state = 1;
			if(!parse_blank(term))
				return 0;
		case 2:
			term->state = 2;
			if(next_char(term) == NO_MORE_READ)
				return 0;
			if(next_char(term) != '\n')
				return syntax_error("unexpected argument",term);
			else
				get_char(term);

	}

	cmd_launch(term);

	return 1;

}

int parse_load(terminal *term)
{
	switch(term->state)
	{
		case 1:
			term->state = 1;
			if(!parse_blank(term))
				return 0;
		case 2:
			term->state = 2;
			if(!parse_str_arg(term,&term->split_cmd))
				return 0;
			if(term->split_cmd.length == 0)
			{
				free_str_arg(&term->split_cmd);
				return syntax_error("expected string",term);
			}
		case 3:
			term->state = 3;
			if(!parse_blank(term))
				return 0;
		case 4:
			term->state = 4;
			if(next_char(term) == NO_MORE_READ)
				return 0;
			if(next_char(term) != '\n')
			{
				free_str_arg(&term->split_cmd);
				return syntax_error("unexpected argument",term);
			}else
				get_char(term);
	}

	cmd_load(term);

	free_str_arg(&term->split_cmd);
	
	return 1;
}

int parse_save(terminal *term)
{
	switch(term->state)
	{
		case 1:
			term->state = 1;
			if(!parse_blank(term))
				return 0;
		case 2:
			term->state = 2;
			if(!parse_str_arg(term,&term->split_cmd))
				return 0;
			if(term->split_cmd.length == 0)
			{
				free_str_arg(&term->split_cmd);
				return syntax_error("expected string",term);
			}
		case 3:
			term->state = 3;
			if(!parse_blank(term))
				return 0;
		case 4:
			term->state = 4;
			if(next_char(term) == NO_MORE_READ)
				return 0;
			if(next_char(term) != '\n')
			{
				free_str_arg(&term->split_cmd);
				return syntax_error("unexpected argument",term);
			}else
				get_char(term);
	}

	cmd_save(term);

	free_str_arg(&term->split_cmd);
	
	return 1;
}

int parse_show_aquarium(terminal *term)
{
	switch(term->state)
	{
		case 1:
			term->state = 1;
			if(!parse_blank(term))
				return 0;
		case 2:
			term->state = 2;
			if(!parse_str_arg(term,&term->split_cmd))
				return 0;
			if(term->split_cmd.length != 8 || strncmp(term->split_cmd.str,"aquarium",8) != 0)
			{
				free_str_arg(&term->split_cmd);
				return syntax_error("Unknow command",term);
			}
		case 3:
			term->state = 3;
			if(!parse_blank(term))
				return 0;
		case 4:
			term->state = 4;
			if(next_char(term) == NO_MORE_READ)
				return 0;
			if(next_char(term) != '\n')
			{
				free_str_arg(&term->split_cmd);
				return syntax_error("unexpected argument",term);
			}else
				get_char(term);

	}

	cmd_show_aquarium(term);

	free_str_arg(&term->split_cmd);
	
	return 1;
}

int parse_del_view(terminal *term)
{
	switch(term->state)
	{
		case 1:
			term->state = 1;
			if(!parse_blank(term))
				return 0;
		case 2:
			term->state = 2;
			if(!parse_str_arg(term,&term->split_cmd))
				return 0;
			if(term->split_cmd.length != 4 || strncmp(term->split_cmd.str,"view",4) != 0)
			{
				free_str_arg(&term->split_cmd);
				return syntax_error("Unknow command",term);
			}
			free_str_arg(&term->split_cmd);
		case 3:
			term->state = 3;
			if(!parse_blank(term))
				return 0;
		case 4:
			term->state = 4;
			if(!parse_str_arg(term,&term->cv.id))
				return 0;
			if(term->cv.id.length == 0)
			{
				free_str_arg(&term->cv.id);
				return syntax_error("Expected string",term);
			}
		case 5:
			term->state = 5;
			if(!parse_blank(term))
				return 0;
		case 6:
			term->state = 6;
			if(next_char(term) == NO_MORE_READ)
				return 0;
			if(next_char(term) != '\n')
			{
				free_str_arg(&term->cv.id);
				return syntax_error("unexpected argument",term);
			}else
				get_char(term);


	}

	cmd_del_view(term);

	free_str_arg(&term->cv.id);
	
	return 1;
}

int parse_add_view(terminal *term)
{
	switch(term->state)
	{
		case 1:
			term->state = 1;
			if(!parse_blank(term))
				return 0;
		case 2:
			term->state = 2;
			if(!parse_str_arg(term,&term->split_cmd))
				return 0;
			if(term->split_cmd.length != 4 || strncmp(term->split_cmd.str,"view",4) != 0)
			{
				free_str_arg(&term->split_cmd);
				return syntax_error("Unknow command",term);
			}
			free_str_arg(&term->split_cmd);
		case 3:
			term->state = 3;
			if(!parse_blank(term))
				return 0;
		case 4:
			term->state = 4;
			if(!parse_str_arg(term,&term->cv.id))
				return 0;
			if(term->cv.id.length == 0)
			{
				free_str_arg(&term->cv.id);
				return syntax_error("Expected string",term);
			}
		case 5:
			term->state = 5;
			if(!parse_blank(term))
				return 0;
		case 6:
			term->state = 6;
			if(!parse_int(term,&term->cv.pos_x))
				return 0;
			if(term->cv.pos_x == -1)
			{
				free_str_arg(&term->cv.id);
				return syntax_error("Expected number",term);
			}
		case 7:
			term->state = 7;
			if(next_char(term) == NO_MORE_READ)
				return 0;
			if(next_char(term) != 'x')
			{
				free_str_arg(&term->cv.id);
				return syntax_error("expected x",term);
			}else
				get_char(term);
		case 8:
			term->state = 8;
			if(!parse_int(term,&term->cv.pos_y))
				return 0;
			if(term->cv.pos_y == -1)
			{
				free_str_arg(&term->cv.id);
				return syntax_error("Expected number",term);
			}
		case 9:
			term->state = 9;
			if(next_char(term) == NO_MORE_READ)
				return 0;
			if(next_char(term) != '+')
			{
				free_str_arg(&term->cv.id);
				return syntax_error("expected +",term);
			}else
				get_char(term);
		case 10:
			term->state = 10;
			if(!parse_int(term,&term->cv.width))
				return 0;
			if(term->cv.width == -1)
			{
				free_str_arg(&term->cv.id);
				return syntax_error("Expected number",term);
			}
		case 11:
			term->state = 11;
			if(next_char(term) == NO_MORE_READ)
				return 0;
			if(next_char(term) != '+')
			{
				free_str_arg(&term->cv.id);
				return syntax_error("expected +",term);
			}else
				get_char(term);
		case 12:
			term->state = 12;
			if(!parse_int(term,&term->cv.height))
				return 0;
			if(term->cv.height == -1)
			{
				free_str_arg(&term->cv.id);
				return syntax_error("Expected number",term);
			}
		case 13:
			term->state = 13;
			if(next_char(term) == NO_MORE_READ)
				return 0;
			if(next_char(term) != '\n')
			{
				free_str_arg(&term->cv.id);
				return syntax_error("unexpected argument",term);
			}else
				get_char(term);


	}

	cmd_add_view(term);

	free_str_arg(&term->cv.id);
	
	return 1;
}

int parse_exit(terminal *term)
{
	switch(term->state)
	{
		case 1:
			term->state = 1;
			if(!parse_blank(term))
				return 0;
		case 2:
			term->state = 2;
			if(next_char(term) == NO_MORE_READ)
				return 0;
			if(next_char(term) != '\n')
				return syntax_error("unexpected argument",term);
			else
				get_char(term);

	}

	term->is_killed = 1;
	
	return 1;
}

//command functions
void cmd_load(terminal *term)
{
	char *filename;
	int i;

	filename = malloc(term->split_cmd.length+1); // + '\0'

	for(i=0;i<term->split_cmd.length;i++)
		filename[i] = term->split_cmd.str[i];

	filename[i] = '\0';

	if(parse_aquarium(filename,&(term->serv.aqua)))
	{
		printf("enable to parse this file\n");
	}else
		printf("Aquarium loaded\n");

	free(filename);
}

void cmd_save(terminal *term)
{
	char *filename,number[16];
	int i,file;

	filename = malloc(term->split_cmd.length+1); // + '\0'

	for(i=0;i<term->split_cmd.length;i++)
		filename[i] = term->split_cmd.str[i];

	filename[i] = '\0';

	file = open(filename,O_WRONLY|O_CREAT,S_IRWXU);

	if(file != -1)
	{
		write(file,number,sprintf("%d",term->serv.aqua.size.width));
		write(file,1,"x");
		write(file,number,sprintf("%d",term->serv.aqua.size.height));
		write(file,1,"\n");

		for(i=0;i<term->serv.aqua.nb_views;i++)
		{
			write(file,1,"N");
			write(file,number,sprintf("%d",i));
			write(file,1," ");
			write(file,number,term->serv.aqua.views[i].pos.x);
			write(file,1,"x");
			write(file,number,term->serv.aqua.views[i].pos.y);
			write(file,1,"+");
			write(file,number,term->serv.aqua.views[i].size.width);
			write(file,1,"+");
			write(file,number,term->serv.aqua.views[i].size.height);
			write(file,1,"\n");

		}
		printf("Aquarium saved !(%d display view)\n",i);
		close(file);
	}else
	{
		perror("open()");
		printf("can't open file\n");
	}

	free(filename);
}

void cmd_show_aquarium(terminal *term)
{
	int i;

	write(1,"   __\n",6);
	write(1,"|\\/ *\\\n",7);
	write(1,"|/\\__/\n",7);

	printf("\n%dx%d\n",term->serv.aqua.size.width,term->serv.aqua.size.height);

	for(i=0;i<term->serv.aqua.nb_views;i++)
	{
		printf("N%d %dx%d+%d+%d\n",i+1,
			term->serv.aqua.views[i].pos.x,
			term->serv.aqua.views[i].pos.y,
			term->serv.aqua.views[i].size.width,
			term->serv.aqua.views[i].size.height);
	}
}

void cmd_add_view(terminal *term)
{
	write(1,"adding view : ",14);
	write(1,term->cv.id.str,term->cv.id.length);
	write(1,"\n",1);
	
	printf("pos x : %d\n",term->cv.pos_x);
	printf("pos y : %d\n",term->cv.pos_y);
	printf("width : %d\n",term->cv.width);
	printf("height : %d\n",term->cv.height);

}

void cmd_del_view(terminal *term)
{
	write(1,"deleting view : ",16);
	write(1,term->cv.id.str,term->cv.id.length);
	write(1,"\n",1);
}

void cmd_launch(terminal *term)
{
	struct sockaddr_in listen_addr;

	if(term->serv.socket != -1)
	{
		printf("server already launched\n");
		return;
	}

	term->serv.socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_addr.sin_port = htons(term->serv.conf.controller_port);
	listen_addr.sin_family = AF_INET;

	if(bind(term->serv.socket,(struct sockaddr*)&listen_addr,sizeof(struct sockaddr_in)) == -1)
	{
		perror("bind()");
		term->serv.socket = -1;
		return;
	}

	if(listen(term->serv.socket,term->serv.conf.max_client) == -1)
	{
		perror("listen()");
		term->serv.socket = -1;
		return;
	}
}
