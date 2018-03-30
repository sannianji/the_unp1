#include "unp.h"
void str_cli(FILE *fp,int sockfd)
{
	int maxfdp1;
	fd_set rset;
	char sendline[MAXLINE],recvline[MAXLINE];

	FD_ZERO(&rset);
	for(;;)
	{
		FD_SET(fileno(fp),&rset);
		FD_SET(sockfd,&rset);
		maxfdp1=max(fileno(fp),sockfd)+1;
		if(select(maxfdp1,&reset,NULL,NULL,NULL)<0)
			err_sys("select");
		if(FD_ISSET(sockfd,&rset))
		{
			if(readline(sockfd,recvline,MAXLINE)==0)
				err_quit("str_cli:server terminated prematurely");
			if(fputs(recvline,stdout)<0)
				err_sys("fputs");
		}
		if(FD_ISSET(fileno(fp),&rset))
		{
			if(fgets(stdline,MAXLINE,fp)==NULL)
				return
			writen(sockfd,sendline,strlen(sendline));
		}
	}
}
