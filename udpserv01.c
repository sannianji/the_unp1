#include "unp.h"
extern void dg_echo(int,SA *,socklen_t);
int main(int argc,char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr,cliaddr;

	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
		err_sys("socket");
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(SERV_PORT);

	if(bind(sockfd,(SA *)&servaddr,sizeof(servaddr))<0)
		err_sys("bind");

	dg_echo(sockfd,(SA *)&cliaddr,sizeof(cliaddr));
}

