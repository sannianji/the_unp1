#include "unp.h"
void str_cli(FILE* fp,int sockfd)
{
	char sendlin[MAXLINE],recvline[MAXLINE];
	while(fgets(sendlin,MAXLINE,fp)!=NULL)
	{
		if(writen(sockfd,sendlin,strlen(sendlin))<0)
			err_sys("writen");
		if(readline2(sockfd,recvline,MAXLINE)==0)
			err_quit("str_cli: server terminated prematurely");
		if(fputs(recvline,stdout)==EOF)
			err_sys("fputs");
	}
}
