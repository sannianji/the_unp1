#include "unp.h"
void dg_cli(FILE *fp,int sockfd,const SA *pservaddr,socklen_t servlen)
{
	int n;
	char sendline[MAXLINE],recvline[MAXLINE+1];
	
	if(connect(sockfd,(SA *)pservaddr,servlen)<0)
		err_sys("connect");
	
	while(fgets(sendline,MAXLINE,fp)!=NULL)
	{
		if(write(sockfd,sendline,strlen(sendline))!=strlen(sendline))
			err_sys("write");
	
		if((n=read(sockfd,recvline,MAXLINE))<0)
			err_sys("read");
			
		recvline[n]=0;
		fputs(recvline,stdout);
	}
}
