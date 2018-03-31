#include "unp.h"
extern void dg_cli(FILE*,int,SA *,socklen_t);
int main(int argc,char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	
	if(argc!=2)
		err_quit("usage:udpcli <IPaddress>");
	
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(SERV_PORT);
	inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
	

	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
		err_sys("socket");
	dg_cli(stdin,sockfd,(SA *)&servaddr,sizeof(servaddr));
	exit(0);
}
