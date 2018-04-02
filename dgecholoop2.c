#include "unp.h"

static void recvfrom_init(int);
static int count;

void dg_echo(int sockfd,SA *pcliaddr,socklen_t clilen)
{
	int n;
	socklen_t len;
	char mesg[MAXLINE];

	if(signal(SIGINT,recvfrom_init)<0)
		err_sys("signal");

//	n=220*1024;
	
//	if(setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&n,sizeof(n))<0)
//		err_sys("setsockopt");

	for(;;)
	{
		len=clilen;
		if(recvfrom(sockfd,mesg,MAXLINE,0,pcliaddr,&len)<0)
			err_sys("recvfrom");
		count++;
		printf("recieved %d\n",count);
		fflush(stdout);
	}
}
static void recvfrom_init(int signo)
{
	printf("\nreceived %d datagrams\n",count);
	exit(0);
}
