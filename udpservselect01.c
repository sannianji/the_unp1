#include "unp.h"
extern void str_echo(int);
int main(int argc,char **argv)
{
	int listenfd,connfd,udpfd,nready,maxfdp1;
	char mesg[MAXLINE];
	pid_t childpid;
	fd_set rset;
	ssize_t n;
	socklen_t len;
	const int on=1;
	struct sockaddr_in cliaddr,servaddr;
	void sig_child(int);
	
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
		err_sys("socket");

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(SERV_PORT);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0)
		err_sys("setsockopt");
	
	if(bind(listenfd,(SA *)&servaddr,sizeof(servaddr))<0)
		err_sys("bind");

	if(listen(listenfd,LISTENQ)<0)
		err_sys("listen");
	
	if((udpfd=socket(AF_INET,SOCK_DGRAM,0))<0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(SERV_PORT);
	if(bind(udpfd,(SA *)&servaddr,sizeof(servaddr))<0)
		err_sys("bind");
	
	if(signal(SIGCHLD,sig_child)==SIG_ERR)
		err_sys("signal");

	FD_ZERO(&rset);
	maxfdp1=maxfd(listenfd,udpfd)+1;
	for(;;)
	{
		FD_SET(listenfd,&rset);
		FD_SET(udpfd,&rset);
		if((nready=select(maxfdp1,&rset,NULL,NULL,NULL))<0)
		{
			if(errno=EINTR)
				continue;
			else
				err_sys("select");
		}
		if(FD_ISSET(listenfd,&rset))
		{
			len=sizeof(cliaddr);
			if((connfd=accept(listenfd,(SA *)&cliaddr,&len))<0)
				err_sys("accept");
			if((childpid=fork())<0)
				err_sys("fork");
			if(childpid==0)
			{
				if(close(listenfd)<0)
					err_sys("close");
				str_echo(connfd);
				exit(0);
			}
			if(childpid>0)
				if(close(connfd)<0)
					err_sys("close");
		}
		if(FD_ISSET(udpfd,&rset))
		{
			len=sizeof(cliaddr);
			if((n=recvfrom(udpfd,mesg,MAXLINE,0,(SA *)&cliaddr,&len))<0)
				err_sys("recvfrom");
			if(sendto(udpfd,mesg,n,0,(SA *)&cliaddr,len)<0)
				err_sys("sendto");
		}
	}
}
void sig_child(int signo)
{
	pid_t pid;
	int stat;
	while((pid=waitpid(-1,&stat,WNOHANG))>0)
		printf("child %d terminated\n",pid);
	return;
}
