#include "unp.h"
void str_cli(FILE *fp,int sockfd)
{
	int maxfdp1,stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;
		
	stdineof=0;
	FD_ZERO(&rset);
	for(;;)
	{
		if(stdineof==0)
			FD_SET(fileno(fp),&rset);
		FD_SET(sockfd,&rset);
		maxfdp1=maxfd(fileno(fp),sockfd)+1;
		if(select(maxfdp1,&rset,NULL,NULL,NULL)<0)
			err_sys("select");
		if(FD_ISSET(sockfd,&rset))
		{
			if((n=readline(sockfd,buf,MAXLINE))<0)
				err_sys("readline");
			else if(n==0)
			{
				if(stdineof==1)
					return;
				else
					err_quit("str_cli:server terminated prematurely");
			}
			if(write(fileno(stdout),buf,n)!=n)
				err_sys("write");
		}
		if(FD_ISSET(fileno(fp),&rset))
		{
			if((n=read(fileno(fp),buf,MAXLINE))<0)
				err_sys("read");
			else if(n==0)
			{
				stdineof=1;
				if(shutdown(sockfd,SHUT_WR)<0)
					err_sys("shutdown");
				FD_CLR(fileno(fp),&rset);
				continue;
			}
			if(write(sockfd,buf,n)!=n)
				err_sys("write");
		}
	}
}
