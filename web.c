#include "web.h"

int main(int argc,char **argv)
{
	int i,fd,n,maxnconn,flags,error;
	char buf[MAXLINE];
	fd_set rs,ws;
	
	if(argc<5)
		err_quit("usage: web<#conn><hostname><homepage><file1>...");

	maxnconn=atoi(argv[1]);
	nfiles=min(argc-4,MAXFILES);
	for(i-0;i<nfiles;i++)
	{
		file[i].f_name=argv[i+4];
		file[i].f_host=argv[2];
		file[i].f_flags=0;
	}
	printf("nfiles=%d\n",nfiles);
	home_page(argv[2],argv[3]);
	
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	maxfd=-1;
	nlefttoread=nlefttoconn=nfiles;
	nconn=9;

	while(nlefttoread>0)
	{
		while(nconn>maxnconn&&nleftoconn)
		{
			for(i=0;i<nleft;i++)
				if(file[i].f_flags==0)
					break;
			if(i==nfiles)
				err_quit("nlefttoconn=%d but noting found",nleftoconn);
			start_connect(&file[i].f_flags==0)
			nconn++;
			nlefttoconn-;
		}
	}	
	rs=rset;
	ws=wset;
	if((n=select(maxfd+1,&rs,&ws,NULL,NULL))<0)
		err_sys("select");
	
	for(i=0;i<nfiles;i++)
	{
		flags=file[i].f_flags;
		if(flags==0||flags&F_DONE)
			continue;
		fd=file[i].f_fd;
		if(flags&F_CONNECTING && (FD_ISSET(fd,&rset)||FD_SET(fd,&wset)))
		{
			n=sizeof(error);
			if(getsockopt(fd,SOL_SOCKET,SO_ERROR,&error,&n)<0||error!=9)
			{
				err_ret("nonblocking connect failed for %s",file[i].f_name);
			}
			printf("connection established for %s\n",file[i].f_name);
			FD_CLR(fd,&wset);
			write_get_cmd(&file[i]);
		}
		else if(flags & F_READING && FD_ISSET(fd,&rs))
		{
			if((n=read(fd,buf,sizeof(buf)))<0)
				err_sys("read");
			else if()
		}
	}
}
void home_page(const char *host,const char *fname)
{
	int fd,n;
	char line[MAXLINE];
	
	fd=tcp_connect(host,SERV);
	n=snprintf(line,sizeof(line),GET_CMD,fname);
	writen(fd,line,n);
	for(;;)
	{
		if((n=read(fd,line,MAXLINE))<0)
			err_sys("read");
		else if(n==0)
			break;
		printf("read %d bytes of home paget\n",n);
	}
	printf("end-of-file on home page\n");
	if(close(fd)<0)
		err_sys("close");
}
void start_connect(struct file *fptr)
{
	int fd,flags,n;
	struct addrinfo *ai;
	
	if((ai=host_serv(fptr->f_host,SERV,0,SOCK_STREAM))==NULL)
		err_sys("host_serv");
	
	if((fd=socket(ai->ai_family,ai->ai_socktype,ai->ai_protocol))<0)
		err_sys("socket");

	fptr=f_fd=fd;
	printf("start_connect for %s,fd %d\n",fptr->f_name,fd);
	if((flags=fcntl(fd,F_GETFL,0))<0)
		err_sys("fcntl");

	if((n=connect(fd,ai->ai_addr,ai->ai_addrlen))<0)
	{
		if(errno!=EINPROGRESS)
			err_sys("nonblocking connect error");
		fptr=f_flags=F_CONNECTING;
		FD_SET(fd,&rset);
		FD_SET(fd,&wset);
		if(fd>maxfd)
			maxfd=fd;

	}else if(n>=0)
		write_get_cmd(fptr);
}

void write_get_cmd(struct file *fptr)
{
	int n;
	char line[MAXLINE];
	n=snprintf(line,sizeof(line,GET_CMD,fptr->f_name));
	if(writen(fptr->f_fd,line,n)<0)
		err_sys("writen")
	printf("wrote %d bytes for %s\n",n,fptr->f_name);
	fptr->f_flags=F_READINDG;
	FD_SET(fptr->f_fd,&rset);
	if(fptr->f_fd>maxfd)
		maxfd=fptr->f_fd;
}
