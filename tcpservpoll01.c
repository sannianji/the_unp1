#include "unp.h"
int main(int argc,char **argv)
{
	int i,maxi,sockfd,listenfd,connfd,nready;
	struct sockaddr_in cliaddr,servaddr;
	socklen_t clilen;
	char buf[MAXLINE];
	ssize_t n;
	struct pollfd client[OPEN_MAX];
		
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
		err_sys("socket");

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(SERV_PORT);

	if((listenfd=bind(AF_INET,(SA *)&servaddr,sizeof(servaddr)))<0)
		err_sys("bind");
	if(listen(listenfd,LISTENQ)<0)
		err_sys("listenfd");
	client[0].fd=listenfd;
	client[0].events=POLLRDNORM;
	for(i=1;i<OPEN_MAX;i++)
		client[i].fd=-1;
	maxi=0;
	for(;;)
	{
		if((nready=poll(client,maxi+1,INFTIM))<0)
			err_sys("poll");
		if(client[0].revents&POLLRDNORM)
		{
			clilen=sizeof(cliaddr);
			if((connfd=accept(listenfd,(SA *)&cliaddr,&clilen))<0)
				err_sys("accept");
	
			for(i=1;i<OPEN_MAX;i++)
			{
				if(client[i].fd<0)
				{
					client[i].fd=connfd;
					break;
				}
			}
			if(i==OPEN_MAX)
				err_quit("too many clients");
			client[i].events=POLLRDNORM;
			if(i>maxi)
				maxi=i;
			if(--nready<=0)
				continue;
		}
		for(i=1;i<=maxi;i++)
		{
			if((sockfd=client[i].fd)<0)
				continue;
			if(client[i].revents & (POLLRDNORM|POLLERR))
			{
				if((n=read(sockfd,buf,MAXLINE))<0)
				{
					if(errno=ECONNRESET)
					{
						if(close(sockfd)<0)
							err_sys("close");
						client[i].fd=-1;
					}
					else
						err_sys("read error");
				}
				else if(n==0)
				{
					if(close(sockfd)<0)
						err_sys("close");
					client[i].fd=-1;
				}
				else 
					writen(sockfd,buf,n);
				if(--nready<=0)
					break;
			}	
		}
	}
}
