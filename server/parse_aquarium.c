#include "parse_aquarium.h"

int next_char_aqua(cursor *cur)
{
	char c;

	if(cur->next_char == NO_CHAR)
	{
		if(read(cur->file,&c,1) != 1)
		{
			cur->next_char = END_OF_FILE;
		}else
			cur->next_char = c;
	}	
	
	return cur->next_char;
}

int get_char_aqua(cursor *cur)
{
	int c;

	c = next_char_aqua(cur);

	cur->next_char = NO_CHAR;

	if(c == '\n')
	{
		cur->line++;
		cur->pos = 0;
	}else
		cur->pos ++;

	return c;
}

int read_number(cursor *cur, int *nbr)
{
	int c,i;
	
	*nbr = 0;

	for(i=0;;i++)
	{
		c = next_char_aqua(cur);

		if(c <= '9' && c >= '0')
		{
			get_char_aqua(cur);
			*nbr = (*nbr)*10 + (c - '0');
		}else
		{
			if(i == 0)
				return 1;
			else
				return 0;
		}
	}
}

int syntax_error_aqua(cursor *cur,char *error,struct aquarium *aq)
{
	free(aq->views);
	printf("line %d:%d %s\n",cur->line,cur->pos,error);
	close(cur->file);
	return 1;
}

int parse_aquarium(char *filename,struct aquarium *aq)
{
	int i,nb;
	char c;
	int nb_alloc;
	cursor cur;

	cur.next_char = NO_CHAR;
	cur.file = open(filename,O_RDONLY);
	cur.pos = 1;
	cur.line = 1;

	if(cur.file == -1)
	{
		perror("open()");
		return 1;
	}

	if(aq->nb_views_allocated == 0)
	{
		nb_alloc = START_ALLOC_NB_VIEW;
		aq->views = (struct view*)malloc(sizeof(struct view)*nb_alloc);
	}else
	{
		nb_alloc = aq->nb_views_allocated;
	}	
	aq->nb_views = 0;
	aq->nb_fishes = 0;

	if(read_number(&cur,&(aq->size.width)))
		return syntax_error_aqua(&cur,"expected number",aq);
	
	if(get_char_aqua(&cur) != 'x')
		return syntax_error_aqua(&cur,"expected  \'x\'",aq);

	if(read_number(&cur,&(aq->size.height)))
		return syntax_error_aqua(&cur,"expected number",aq);

	for(i=1;;i++,aq->nb_views)
	{
		if(next_char_aqua(&cur) == END_OF_FILE)
		{
			close(cur.file);
			return 0;
		}else if(get_char_aqua(&cur) != '\n')
			return syntax_error_aqua(&cur,"expected \'\\n\'",aq);

		if(next_char_aqua(&cur) == END_OF_FILE)
		{
			close(cur.file);
			return 0;
		}

		if(aq->nb_views == nb_alloc)
		{
			nb_alloc *= 2;
			aq->views = (struct view*)realloc(aq->views,sizeof(struct view)*nb_alloc);
		}

		if(get_char_aqua(&cur) != 'N')
			return syntax_error_aqua(&cur,"expected \'N\'",aq);

		if(read_number(&cur,&nb))
			return syntax_error_aqua(&cur,"expected number",aq);

		if(nb != i)
			printf("warning : expected N%d but read N%d\n",i,nb);

		if(get_char_aqua(&cur) != ' ')
			return syntax_error_aqua(&cur,"expected  \' \'",aq);

		if(read_number(&cur,&aq->views[aq->nb_views].pos.x))
			return syntax_error_aqua(&cur,"expected number",aq);

		if(get_char_aqua(&cur) != 'x')
			return syntax_error_aqua(&cur,"expected  \'x\'",aq);

		if(read_number(&cur,&aq->views[aq->nb_views].pos.y))
			return syntax_error_aqua(&cur,"expected number",aq);

		if(get_char_aqua(&cur) != '+')
			return syntax_error_aqua(&cur,"expected  \'+\'",aq);

		if(read_number(&cur,&aq->views[aq->nb_views].size.width))
			return syntax_error_aqua(&cur,"expected number",aq);

		if(get_char_aqua(&cur) != '+')
			return syntax_error_aqua(&cur,"expected  \'+\'",aq);

		if(read_number(&cur,&aq->views[aq->nb_views].size.height))
			return syntax_error_aqua(&cur,"expected number",aq);

		aq->views[aq->nb_views].client = AVAILABLE;
		aq->nb_views++;
	}
}
