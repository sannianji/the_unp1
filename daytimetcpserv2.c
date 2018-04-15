#include "unp.h"
#include <time.h>

int main(int argc,char **argv)
{
	int listenfd,connfd;
	socklen_t addrlen,len;
	struct sockaddr* cliaddr;
	char buff[MAXLINE];
	time_t ticks;

	if(argc<2||argc>3)
		err_quit("usage: daytimetcpserv2 [<host>]<service or port>");

//	daemon_init(argv[0],0);

	if(argc==2)
		if((listenfd=tcp_listen(NULL,argv[1],&addrlen))<0)
			err_sys("tcp_listen");

	else
		if((listenfd=tcp_listen(argv[1],argv[2],&addrlen))<0)
			err_sys("tcp_listen");

	if((cliaddr=malloc(addrlen))==NULL)
		err_sys("malloc");

	for(;;)
	{
		len=addrlen;
		write(STDOUT_FILENO,"1",1);
		if((connfd=accept(listenfd,cliaddr,&len))==-1)
			err_sys("accept");
		write(STDOUT_FILENO,"2",1);
		err_msg("connection from %s",sock_ntop(cliaddr,len));
		ticks=time(NULL);
		snprintf(buff,sizeof(buff),"%.24s\r\n",ctime(&ticks));
		if(write(connfd,buff,strlen(buff))!=strlen(buff))
			err_sys("write");

		if(close(connfd)<0)
			err_sys("close");
	}
}
