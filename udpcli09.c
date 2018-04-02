#include "unp.h"

int main(int argc,char **argv)
{
	int sockfd;
	socklen_t len;
	struct sockaddr_in cliaddr,servaddr;
	
	if(argc!=2)
		err_quit("usage: udpcli <IPaddress>");
	
	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
		err_sys("socket");

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(SERV_PORT);
	if(inet_pton(AF_INET,argv[1],&servaddr.sin_addr)<0)
		err_sys("inet_pton");

	if(connect(sockfd,(SA *)&servaddr,sizeof(servaddr))<0)
		err_sys("connect");

	len=sizeof(cliaddr);
	if(getsockname(sockfd,(SA *)&cliaddr,&len)<0)
		err_sys("getsockname");
	printf("local address %s\n",sock_ntop((SA *)&cliaddr,len));
	exit(0);
}
