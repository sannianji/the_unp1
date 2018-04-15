#include "unp.h"
void str_cli(FILE *fp,int sockfd)
{
	int maxfdp1,val,stdineof;
	ssize_t n,nwritten;
	fd_set rset,wset;
	char to[MAXLINE],fr[MAXLINE];
	char *toiptr,*toopter,*friptr,*froptr;

	if((val=fcntl(sockfd,F_GETFL,0))<0)
		err_sys("fcntl");
	if(fcntl(cockfd,F_SETFL,val|O_NONBLOCK)<0)
		err_sys("fcntl");

	if((val=fcntl(STDIN_FILENO,F_GETFL,0))<0)
		err_sys("fcntl");
	if(fcntl(STDIN_FILENO,F_SETFL,val|O_NONBLOCK)<0)
		err_sys("fcntl");

	if((val=fcntl(STDOUT_FILENO,F_GETFL,0))<0)
		err_sys("fcntl");
	if(fcntl(STDOUT_FILENO,F_SETFL,val|O_NONBLOCK)<0)
		err_sys("fcntl");

	toiptr=tooptr=to;
	friptr=froptr=fr;
	stdineof=0;
	maxfdp1=max(max(STDOUT_FILENO,STDIN_FILENO),sockfd)+1;

	for(;;)
	{
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		
		if(stdineof==0&&toiptr<&to[MAXLINE])
		{
			FD_SET(STDIN_FILENO,&rset);
		}
		if(frptr<&fr[MAXLINE])
		{
			FD_SET(sockfd,&rset);
		}
		if(tooptr!=toiptr)
		{
			FD_SET(sockfd,&wset);
		}
		if(froptr!=friptr)
		{
			FD_SET(STDOUT_FILENO,&wset);
		}

		if(select(maxfdp1,&rset,&wset,NULL,NULL)<0)
			err_sys("select");

		if(FD_ISSET(STDIN_FILENO,&rset))
		{
			if((n=read(STDIN_FILENO,toiptr,&to[MAXLINE]-toiptr))<0)
				if(errno!=EWOULDBLOCK)
					err_sys("read error on stdin");
			else if(n==0)
			{
				fprintf(stderr,"%s: EOF on stdin\n",gf_time());
			}
		}
		if(FD_ISSET(sockfd,&rset))
		{
			if((n=read(sockfd,friptr,&fr[MAXLINE]-friptr))<0)
			{
				if(errno!=EWOULDBLOCK)
					err_sys("read");
			}
			else if(n==0)
			{
				fprintf(stderr,"%s:EOF on socket\n",gf_time());
				if(stdineof)
					return;
				else
					err_quit("str_cli:server terminated prematurely");
			}
			else
			{
				fnprintf(stderr,"%s: read %d bytes from socket\n",gr_time(),n);
			friptr+=n;
			FD_SET(STDOUT_FILENO,&wset);
			}
		}
		if(FD_ISSET(STDOUT_FILENO,&wset)&&((n=friptr-froptr)>0))
		{
			if((nwritten=write(STDOUT_FILENO,fropt,n))<0)
			{
				if(errno!=EWOULDBLOCK)
					err_sys("write error to stdout");
			}
		}	
	}
}
