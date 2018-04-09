#include "unp.h"
#include <time.h>

int main(int argc,char **argv)
{
	int listenfd,connfd;
	socklen_t len;
	char buff[MAXLINE];
	time_t ticks;
	struct sockaddr_storage cliaddr;

	if(argc!=2)
		err_quit("usage daytimetcpserv <service or port>");

	listenfd=tcp_listen(NULL,argv[1],NULL);

	for(;;)
	{
		len=sizeof(cliaddr);
		if((connfd=accept(listenfd,(SA *)&cliaddr,&len))<0)
			err_sys("accept");
		printf("connection fron %s\n",sock_ntop((SA*)&cliaddr,len));
		
		ticks=time(NULL);
		snprintf(buff,sizeof(buff),"%.24s\r\n",ctime(&ticks));
		
		if((n=write(connfd,buff,strlen(buff)))!=strlen(buff))
		{
			printf("n=%d,strlen(buff)=%d\n",n,strlen(buff))
		}
		if(close(connfd)<0)
			err_sys("close");
	}
}
