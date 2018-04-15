#include "unp.h"

void str_cli(FILE *fp,int sockfd)
{
	pid_t pid;
	char sendline[MAXLINE],recvline[MAXLINE];

	if((pid=fork())<0)
		err_sys("fork");
	else if(pid==0)//child
	{
		while(readline(sockfd,recvline,MAXLINE)>0)
			if(fputs(recvline,stdout)<0)
				err_sys("fputs");
		kill(getppid(),SIGTERM);
		exit(0);
	}
	while(fgets(sendline,MAXLINE,fp)!=NULL)//parent
		if(writen(sockfd,sendline,strlen(sendline))<0)
			err_sys("write");
	if(shutdown(sockfd,SHUT_WR)<0)
		err_sys("shutdown");
	pause();
	return;
}
