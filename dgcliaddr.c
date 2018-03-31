#include "unp.h"

void dg_cli(FILE *fp,int sockfd,const SA *pservaddr,socklen_t servlen)
{
	int n;
	char sendline[MAXLINE],recvline[MAXLINE+1];
	socklen_t len;
	struct sockaddr *preply_addr;
		
	if((preply_addr=malloc(servlen))==NULL)
		err_sys("malloc");

	while(fgets(sendline,MAXLINE,fp)!=NULL)
	{
		if(sendto(sockfd,sendline,strlen(sendline),0,pservaddr,servlen)!=strlen(sendline))
			err_sys("sendto");
	
		len=servlen;
		if((n=recvfrom(sockfd,recvline,MAXLINE,0,preply_addr,&len))<0)
			err_sys("recvfrom");

		if(len!=servlen||memcmp(pservaddr,preply_addr,len)!=0)
		{
				printf("reply from %s (ingored)\n",sock_ntop(preply_addr,len));
				continue;
		}
		recvline[n]=0;
		fputs(recvline,stdout);
	}
}
