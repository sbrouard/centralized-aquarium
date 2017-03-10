#include "server.h"
#include "terminal.h"
#include "config.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int nfds;
	terminal term;
	fd_set read_fds;

	init_server(&term.serv);
	init_terminal(&term);
	parse_config_file(&term.serv.conf);

	while(!term.is_killed)
	{
		FD_ZERO(&read_fds);
		FD_SET(0,&read_fds);

		nfds = fd_to_read(&term.serv,&read_fds);
		
		if(select(nfds,&read_fds,NULL,NULL,NULL) == -1)
		{
			perror("select()");
			return 1;
		}

		read_server(&term.serv,&read_fds);
		read_terminal(&term);
	}
}
