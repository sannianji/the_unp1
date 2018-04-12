#include "unp.h"
#include <time.h>
int main(int argc,char **argv)
{
	socklen_t len;
	struct sockaddr* cliaddr;
	char buff[MAXLINE];
	time_t ticks;

	daemon_inetd(argv[0],0);
	if((cliaddr=malloc(sizeof(struct sockaddr_storage)))==NULL)
		err_sys("malloc");

	len=sizeof(struct sockaddr_storage);

	if(getpeername(0,cliaddr,&len)==-1)
		err_sys("getpeername");
	err_msg("connection from %s",sock_ntop(cliaddr,len));

	ticks=time(NULL);
	snprintf(buff,,sizeof(buff),"%.24s\r\n",ctime(&ticks));
	if(write(0,buff,strlen(buff))!=strlen(buff))
		err_sys("write");
	
	if(close(0)<0)
		err_sys("close");
	exit(0);
}
