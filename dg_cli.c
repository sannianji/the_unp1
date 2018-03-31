#include "unp.h"
void dg_cli(FILE *fp,int sockfd,const SA *pservaddr,socklen_t servlen)
{
	int n;
	char sendlin[MAXLINE],recvline[MAXLINE+1];
	
	while(fgets(sendline,MAXLINE,fp)!=NULL)
	{
		if(sendto(sockfd,sendline,strlen(sendline),0pservaddr,servlen)!=strlen(sendline))
			err_sys("sendto");

		if((n=recvfrom(sockfd,recvline,MAXLINE,0,NULL,NULL))<0)
			err_sys("recvfrom");

		recvline[n]=0;
		fputs(recvline,stdout);
	}
}
