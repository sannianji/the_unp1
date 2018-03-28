#include "unp.h"
char *sock_ntop(const struct sockaddr *sa,socklen_t salen)
{
	char portstr[8];
	static char str[128];
	switch(sa->sa_family)
	{
		case AF_INET:
		{
			struct sockaddr_in *sin=(struct sockaddr_in*)sa;
		
			if(inet_ntop(AF_INET,&sin->sin_addr,str,sizeof(str))==NULL)
				return NULL;
			if(ntohs(sin->sin_port)!=0)
			{
				snprintf(portstr,sizeof(portstr),":%d",ntohs(sin->sin_port));
				strcat(str,portstr);
			}
			return str;
		}
	}
}
ssize_t writen(int fd,const void *vptr,size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	
	ptr=vptr;
	nleft=n;
	while(nleft>0)
	{
		if((nwritten=write(fd,ptr,n))<=0)
		{
			if(nwritten<0&&errno==EINTR)
				nwritten=0;
			else
				return (-1);
		}
		nleft-=nwritten;
		ptr+=nwritten;
	}
	return n;
}
ssize_t readn(int fd,void *vptr,size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;
	nleft=n;
	ptr=vptr;
	while(nleft>0)
	{
		if((nread=read(fd,ptr,nleft))<0)
		{
			if(errno==EINTR)
				nread=0;
			else
				return -1;
		}
		else if(nread==0)
			break;
		nleft-=nread;
		ptr+=nread;
	}
	return (n-nleft);
}
ssize_t readline(int fd,void *vptr,size_t maxlen)
{
	ssize_t n,rc;
	char c,*ptr;
	
	ptr=vptr;
	for(n=1;n<maxlen;n++)
	{
	again:
		if((rc=read(fd,&c,1))==1)
		{
			*ptr++=c;
			if(c=='\n')
				break;
		}
		else if(rc==0)
		{
			*ptr=0;
			return(n-1);
		}
		else
		{
			if(errno==EINTR)
				goto again;
			return -1;
		}
	}
	*ptr=0;
	return n;
}
static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t my_read(int fd,char *ptr)
{
	if(read_cnt<=0)
	{
	again:
		if((read_cnt=read(fd,read_buf,sizeof(read_buf)))<0)
		{
			if(errno=EINTR)
				goto again;
			return -1;
		}
		else if(read_cnt==0)
			return 0;
		read_ptr=read_buf;
	}
	read_cnt--;
	*ptr=*read_ptr++;
	return 1;
}

ssize_t readline2(int fd,void *vptr,size_t maxlen)
{
	ssize_t n,rc;
	char c,*ptr;
	
	ptr=vptr;
	for(n=1;n<maxlen;n++)
	{
		if((rc=my_read(fd,&c))==1)
		{
			*ptr++=c;
			if(c=='\n')
				break;
		}
		else if(rc==0)
		{
			*ptr=0;
			return (n-1);
		}
		else 
			return -1;
	}
	*ptr=0;
	return n;
}
ssize_t readlinebuf(void **vptrptr)
{
	if(read_cnt)
		*vptrptr=read_ptr;
	return read_cnt;
}
void err_sys(const char *str,...)
{
	va_list ap;
	va_start(ap,str);
	char buff[MAXLINE];
	vsnprintf(buff,MAXLINE,str,ap);
	va_end(ap);
	snprintf(buff+strlen(buff),sizeof(buff)-strlen(buff)," error:%s",strerror(errno));
	if(buff[strlen(buff)-1]!='\n')
		strcat(buff,"\n");
	fflush(stdout);
	fputs(buff,stderr);
	fflush(stderr);
	exit(-1);
}
void err_quit(const char *str,...)
{
	va_list ap;
	va_start(ap,str);
	char buff[MAXLINE];
	vsnprintf(buff,MAXLINE,str,ap);
	va_end(ap);
	if(buff[strlen(buff)-1]!='\n')
		strcat(buff,"\n");
	fflush(stdout);
	fputs(buff,stderr);
	fflush(stderr);
	exit(-1);
}
int sockfd_to_family(int sockfd)
{
	struct sockaddr_storage ss;
	socklen_t len;
	len=sizeof(ss);
	if(getsockname(sockfd,(SA*)&ss,&len)<0)
		return -1;
	return ss.ss_family;
}
