#include "unp.h"

int main(int argc,char **argv)
{
	int i,maxi,maxfd,listenfd,connfd,sockfd;
	int nready,client[FD_SETSIZE];
	ssize_t n;
	fd_set rset,allset;
	char buf[MAXLINE];
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

	maxfd=listenfd;
	maxi=-1;
	for(i=0;i<FD_SETSIZE;i++)
		client[i]=-1;
	FD_ZERO(&allset);
	FD_SET(listenfd,&allset);

	for(;;)
	{
		rset=allset;
		if((nready=select(maxfd+1,&rset,NULL,NULL,NULL))<0)
			err_sys("select");
		if(FD_ISSET(listenfd,&rset))
		{
			clilen=sizeof(cliaddr);
			if((connfd=accept(listenfd,(SA *)&cliaddr,&clilen))<0)
				err_sys("accept");
			for(i=0;i<FD_SETSIZE;i++)
			{
				if(client[i]<0)
				{
					client[i]=connfd;
					break;
				}
			}
			if(i==FD_SETSIZE)
				err_quit("too many clients");
			FD_SET(connfd,&allset);
			if(connfd>maxfd)
				maxfd=connfd;
			if(i>maxfd)
				maxi=i;
			if(--nready<=0)
				continue;
		}
		for(i=0;i<=maxi;i++)
		{
			if((sockfd=client[i])<0)
				continue;
			if(FD_ISSET(sockfd,&rset))
			{
				if((n=readline(sockfd,buf,MAXLINE))<0)
					err_sys("readline");
				else if(n==0)
				{
					if(close(sockfd)<0)
						err_sys("close");
					FD_CLR(sockfd,&allset);
					client[i]=-1;
				}
				else 
					if(write(sockfd,buf,n)!=n)
						err_sys("write");
				if(--nready<=0)
					break;
			}
		}
	}
}
