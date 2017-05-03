#include "server.h"
#include "terminal.h"
#include "config.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char **argv)
{
	int nfds,i;
	terminal term;
	fd_set read_fds;
	struct timeval t1,t2,timeout;
	long sec,usec;

	srand(time(NULL));
	init_server(&term.serv);
	init_terminal(&term);

	while(!term.is_killed)
	{
		FD_ZERO(&read_fds);
		FD_SET(0,&read_fds);

		nfds = fd_to_read(&term.serv,&read_fds,&timeout);
		
		gettimeofday(&t1,NULL);
		if(select(nfds,&read_fds,NULL,NULL,&timeout) == -1)
		{
			perror("select()");
			return 1;
		}
		gettimeofday(&t2,NULL);
		sec = t2.tv_sec - t1.tv_sec;
		usec = t2.tv_usec - t1.tv_usec;
		read_terminal(&term);

		if(term.serv.socket != -1)
		{
		if(term.serv.update_fishes.tv_sec < sec)
		{
		  printf("Les poissons bougent\n");
			moveFishes(&term.serv);
			term.serv.update_fishes.tv_sec = INTERVAL_UPDATE_FISHES;
			term.serv.update_fishes.tv_usec = 0;
		}else
		{
			term.serv.update_fishes.tv_sec -= sec;
			if(term.serv.update_fishes.tv_usec <= usec)
			{
				if(term.serv.update_fishes.tv_sec == 0)
				{
				   printf("Les poissons bougent\n");
					moveFishes(&term.serv);
					term.serv.update_fishes.tv_sec = INTERVAL_UPDATE_FISHES;
					term.serv.update_fishes.tv_usec = 0;
				}else
				{
					term.serv.update_fishes.tv_sec--;
					//99999 pour éviter que tv_sec = 1000000
					term.serv.update_fishes.tv_usec = 999999 + term.serv.update_fishes.tv_usec - usec;
				}

			}else
			{
				term.serv.update_fishes.tv_usec -= usec;
			}
		}
		}

		for(i=0;i<term.serv.nb_client;i++)
		{
			if(term.serv.client_list[i].update_continuously)
			{
				if(term.serv.client_list[i].give_continuously.tv_sec < sec)
				{
					printf("send fishes continuously2 %d\n",term.serv.conf.fish_update_interval);
       					sendFishesOfView(&term.serv.client_list[i],&term.serv);
					term.serv.client_list[i].give_continuously.tv_sec = term.serv.conf.fish_update_interval;
					term.serv.client_list[i].give_continuously.tv_usec = 0;
				}else
				{
					term.serv.client_list[i].give_continuously.tv_sec -= sec;
					if(term.serv.client_list[i].give_continuously.tv_usec <= usec)
					{
						if(term.serv.client_list[i].give_continuously.tv_sec == 0)
						{
							printf("send fishes continuously %d\n",term.serv.conf.fish_update_interval);
       							sendFishesOfView(&term.serv.client_list[i],&term.serv);
							term.serv.client_list[i].give_continuously.tv_sec = term.serv.conf.fish_update_interval;
							term.serv.client_list[i].give_continuously.tv_usec = 0;
						}else
						{
							term.serv.client_list[i].give_continuously.tv_sec--;
							//99999 pour éviter que tv_sec = 1000000
							term.serv.client_list[i].give_continuously.tv_usec = 999999 + term.serv.client_list[i].give_continuously.tv_usec - usec;
						}

					}else
					{
						term.serv.client_list[i].give_continuously.tv_usec -= usec;
					}
				}
			}
			if(term.serv.client_list[i].last_activity.tv_sec < sec)
			{
				printf("client disconnected\n");
				remove_client(&term.serv.client_list[i],&term.serv);
				term.serv.client_list[i].last_activity.tv_sec = term.serv.conf.fish_update_interval;
				term.serv.client_list[i].last_activity.tv_usec = 0;
			}else
			{
				term.serv.client_list[i].last_activity.tv_sec -= sec;
				if(term.serv.client_list[i].last_activity.tv_usec <= usec)
				{
					if(term.serv.client_list[i].last_activity.tv_sec == 0)
					{
						printf("client disconnected\n");
						remove_client(&term.serv.client_list[i],&term.serv);
						term.serv.client_list[i].last_activity.tv_sec = term.serv.conf.fish_update_interval;
						term.serv.client_list[i].last_activity.tv_usec = 0;
					}else
					{
						term.serv.client_list[i].last_activity.tv_sec--;
						//99999 pour éviter que tv_sec = 1000000
						term.serv.client_list[i].last_activity.tv_usec = 999999 + term.serv.client_list[i].last_activity.tv_usec - usec;
					}

				}else
				{
					term.serv.client_list[i].last_activity.tv_usec -= usec;
				}
			}
		}

		read_server(&term.serv,&read_fds);
	}
}
