#include "terminal.h"

//ajouter des ' ' pour les commande pour remplire les MAX_SIZE_CMD caractéres
#define NBR_CMD 2
command_function cmd_func[]={{"load  ",parse_load},{"exit  ",parse_exit}};

int init_terminal(terminal *term)
{
	term->command_length = 0;
	term->state = 0;
	term->is_next_get = 0;
	term->caracter_count = 0;
	term->is_killed = 0;

	fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

	term->cl.filename.length = 0;
	term->cl.filename.malloc_size = 0;
	term->cl.filename.str = NULL;
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
	printf("%d : %s\n",term->caracter_count,error);
	term->function = flush_cmd;
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

//command parsing functions
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
			if(!parse_str_arg(term,&term->cl.filename))
				return 0;
			if(term->cl.filename.length == 0)
				return syntax_error("expected string",term);
		case 3:
			term->state = 3;
			if(!parse_blank(term))
				return 0;
		case 4:
			term->state = 4;
			if(next_char(term) == NO_MORE_READ)
				return 0;
			if(get_char(term) != '\n')
				return syntax_error("unexpected argument",term);
	}

	cmd_load(term);

	term->cl.filename.length = 0;
	term->cl.filename.malloc_size = 0;
	free(term->cl.filename.str);
	term->cl.filename.str = NULL;
	
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
			if(get_char(term) != '\n')
				return syntax_error("unexpected argument",term);
	}

	term->is_killed = 1;
	
	return 1;
}

//command functions
void cmd_load(terminal *term)
{
	write(1,"loadind file : ",15);
	write(1,term->cl.filename.str,term->cl.filename.length);
	write(1,"\n",1);
}
