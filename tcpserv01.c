#include "unp.h"
extern void str_echo(int);
int main(int argc,char **argv)
{
	int listenfd,connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr,servaddr;
	
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
		err_sys("socket");
	
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(SERV_PORT);
	
	if(bind(listenfd,(SA *)&servaddr,sizeof(servaddr))<0)
		err_sys("bind");
	if(listen(listenfd,LISTENQ)<0)
		err_sys("listen");	
	for(;;)
	{
		clilen=sizeof(cliaddr);
		if((connfd=accept(listenfd,(SA *)&cliaddr,&clilen))<0)
			err_sys("accept");
		if((childpid=fork())<0)
			err_sys("fork");
		else if(childpid==0)
		{
			if(close(listenfd)<0)
				err_sys("listen");
//			write(STDOUT_FILENO,"here",5);
			str_echo(connfd);
			exit(0);
		}
		if(childpid>0)
			if(close(connfd)<0)
				err_sys("close");
	}
	
}
