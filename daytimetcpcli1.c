#include "unp.h"

int main(int argc,char **argv)
{
	int sockfd,n;
	char recvline[MAXLINE+1];
	struct sockaddr_in servaddr;
	struct in_addr **pptr;
	struct in_addr *inetaddrp[2];
	struct in_addr inetaddr;
	struct hostent *hp;
	struct servent *sp;
	
	if(argc!=3)
		err_quit("usage: daytimecpcli1 <hostname><service>");	
	if((hp=gethostbyname(argv[1]))==NULL)
	{
		if(inet_aton(argv[1],&inetaddr)==0)
		{
			err_quit("hostname error for %s",argv[1],hstrerror(h_errno));
		}
		else
		{
			inetaddrp[0]=&inetaddr;
			inetaddrp[1]=NULL;
			pptr=inetaddrp;
		}
	}
	else
	{
		pptr=(struct in_addr **)hp->h_addr_list;
	}
	if((sp=getservbyname(argv[2],"tcp"))==NULL)
		err_quit("getservbyname error for %s",argv[2]);
	for(;*pptr!=NULL;pptr++)
	{
		if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
			err_sys("socket");
		bzero(&servaddr,sizeof(servaddr));
		servaddr.sin_family=AF_INET;
		servaddr.sin_port=sp->s_port;
		memcpy(&servaddr.sin_addr,*pptr,sizeof(struct in_addr));

		printf("trying %s\n",sock_ntop((SA *)&servaddr,sizeof(servaddr)));

		if(connect(sockfd,(SA *)&servaddr,sizeof(servaddr))==0)
			break;
		err_ret("connect error");
		close(sockfd);
	}
	if(*pptr==NULL)
		err_quit("unable to connect");
		
	while((n=read(sockfd,recvline,MAXLINE))>0)
	{
		recvline[n]=0;
		if(fputs(recvline,stdout)<0)
			err_sys("fputs");
	}
	if(n<0)
		err_sys("read");
	exit(0);
}
