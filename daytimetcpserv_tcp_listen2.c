#include "unp.h"
#include <time.h>
int main(int argc,char **argv)
{
	int listenfd,connfd;
	socklen_t len,addrlen;
	char buff[MAXLINE];
	time_t ticks;
	struct sockaddr_storage cliaddr;
	if(argc==2)
		listenfd=tcp_listen(NULL,argv[1],&addrlen);
	else if(argc==3)
		listenfd=tcp_listen(argv[1],argv[2],&addrlen);
	else
		err_quit("usage: daytimetcpsrv [<host>] <service or port>");

	for(;;)
	{
		len=sizeof(cliaddr);
		if((connfd=accept(listenfd,(SA *)&cliaddr,&len))<0)
			err_sys("accept");
		printf("connection from %s\n",sock_ntop((SA *)&cliaddr,len));
		ticks=time(NULL);

		snprintf(buff,sizeof(buff),"%.24s\r\n",ctime(&ticks));
		if(write(connfd,buff,strlen(buff))!=strlen(buff))
			err_sys("write");

		if(close(connfd)<0)
			err_sys("close");
	}
}
