#include "unp.h"

static void recvfrom_init(int);
static int count;

void dg_echo(int sockfd,SA *pcliaddr,socklen_t clilen)
{
	socklen_t len;
	char mesg[MAXLINE];
	
	if(signal(SIGINT,recvfrom_init)==SIG_ERR)
		err_sys("signal");

	for(;;)
	{
		len=clilen;
		if(recvfrom(sockfd,mesg,MAXLINE,0,pcliaddr,&len)<0)
			err_sys("recvfrom");
		count++;
	}
}

static void recvfrom_init(int signo)
{
	printf("\nreceived %d datagrams\n",count);
	exit(0);
}
