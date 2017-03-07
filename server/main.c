#include "server.h"
#include "terminal.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int nfds;
	server serv;
	fd_set read_fds;

	init_server(&serv);

	while(!serv.term.is_killed)
	{
		FD_ZERO(&read_fds);

		nfds = fd_to_read(&serv,&read_fds);
		
		if(select(nfds,&read_fds,NULL,NULL,NULL) == -1)
		{
			perror("select()");
			return 1;
		}

		read_server(&serv,&read_fds);
	}
}
