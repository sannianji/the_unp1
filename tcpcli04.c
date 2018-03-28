#include "unp.h"
extern void str_cli(FILE*,int);
int main(int argc,char **argv)
{
	int i,sockfd[5];
	struct sockaddr_in servaddr;
	
	if(argc!=2)
		err_quit("usage: tcpcli <IPaddress>");
	for(i=0;i<5;i++)
	{
		if((sockfd[i]=socket(AF_INET,SOCK_STREAM,0))<0)
			err_sys("socket");
		bzero(&servaddr,sizeof(servaddr));
		servaddr.sin_family=AF_INET;
		servaddr.sin_port=htons(SERV_PORT);
		if((inet_pton(AF_INET,argv[1],&servaddr.sin_addr))<0)
			err_sys("inet_pton");
	
		if(connect(sockfd[i],(SA *)&servaddr,sizeof(servaddr))<0)
			err_sys("connect");
	}
	str_cli(stdin,sockfd[0]);
	exit(0);
}
