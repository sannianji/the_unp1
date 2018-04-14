#include "unp.h"
int maxfd(int n1,int n2)
{
	return (n1>n2)?n1:n2;
}
int minfd(int n1,int n2)
{
	return (n1>n2)?n2:n1;
}
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
		case AF_INET6:
		{
			struct sockaddr_in6 *sin=(struct sockaddr_in6 *)sa;

			if(inet_ntop(AF_INET6,&sin->sin6_addr,str,sizeof(str))==NULL)
				return NULL;
			if(ntohs(sin->sin6_port)!=0)
			{
				snprintf(portstr,sizeof(portstr),":%d",ntohs(sin->sin6_port));
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
void err_msg(const char *str,...)
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
	return;
}
void err_ret(const char *str,...)
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

struct addrinfo* host_serv(const char *host,const char *serv,int family,int socktype)
{
	int n;
	struct addrinfo hints,*res;
	
	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family=family;
	hints.ai_flags=AI_CANONNAME;
	hints.ai_socktype=socktype;

	if((n=getaddrinfo(host,serv,&hints,&res))!=0)
		return NULL;

	return res;
}

int tcp_connect(const char *host,const char *serv)
{
	int sockfd,n;
	struct addrinfo hints,*res,*ressave;
	
	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	
	if((n=getaddrinfo(host,serv,&hints,&res))!=0)
		err_quit("tcp_connect error for %s,%s:%s",host,serv,gai_strerror(n));

	ressave=res;
	
	do
	{
		sockfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if(sockfd<0)
			continue;
		if(connect(sockfd,res->ai_addr,res->ai_addrlen)==0)
			break;
		
		if(close(sockfd)<0)
			err_sys("close");

	}while((res=res->ai_next)!=NULL);

	if(res==NULL)
		err_sys("tcp_connect error for %s,%s",host,serv);
	
	freeaddrinfo(ressave);
	
	return sockfd;
}
int tcp_listen(const char *host,const char *serv,socklen_t *addrlenp)
{
	int listenfd,n;
	const int on=1;

	struct addrinfo hints,*res,*ressave;

	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_flags=AI_PASSIVE;
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;

	if((n=getaddrinfo(host,serv,&hints,&res))!=0)
		err_quit("tcp_listen error for %s,%s:%s",host,serv,gai_strerror(n));

	ressave=res;

	do{
		listenfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if(listenfd<0)
			continue;
		if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0)
			err_sys("setsockopt");

		if(bind(listenfd,res->ai_addr,res->ai_addrlen)==0)
			break;

		if(close(listenfd)<0)
			err_sys("close");	
	}while((res=res->ai_next)!=NULL);
	if(res==NULL)
		err_sys("tcp_listen error for %s,%s",host,serv);
	if(listen(listenfd,LISTENQ)<0)
		err_sys("listen");
	if(addrlenp!=NULL)
		*addrlenp=res->ai_addrlen;

	freeaddrinfo(ressave);
	return listenfd;
}

int udp_client(const char *host,const char *serv,SA **saptr,socklen_t *lenp)
{
	int sockfd,n;
	struct addrinfo hints,*res,*ressave;
	
	bzero(&hints,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_DGRAM;
	if((n=getaddrinfo(host,serv,&hints,&res))!=0)
		err_quit("udp_client error for %s,%s",host,serv);
	
	if((*saptr=malloc(res->ai_addrlen))==NULL)
		err_sys("malloc");

	memcpy(*saptr,res->ai_addr,res->ai_addrlen);
	*lenp=res->ai_addrlen;
	freeaddrinfo(ressave);
	return sockfd;
}
int udp_connect(const char *host,const char *serv)
{
	int sockfd,n;
	struct addrinfo hints,*res,*ressave;
	
	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_DGRAM;

	if((n=getaddrinfo(host,serv,&hints,&res))!=0)
		err_quit("udp_connect error for %s,%s:%s",host,serv,gai_strerror(n));

	ressave=res;

	do{
		sockfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
		if(sockfd<0)
			continue;
		if(connect(sockfd,res->ai_addr,res->ai_addrlen)==0)
			break;
		
		if(close(sockfd)<0)
			err_sys("close");
	}while((res=res->ai_next)!=NULL);

	if(res==NULL)
		err_sys("udp_connect error for %s,%s",host,serv);

	freeaddrinfo(ressave);
	return sockfd;
}

int udp_server(const char *host,const char *serv,socklen_t *addrlenp)
{
	int sockfd,n;
	struct addrinfo hints,*res,*ressave;
	
	bzero(&hints,sizeof(struct addrinfo));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_flags=AI_PASSIVE;

	if((n=getaddrinfo(host,serv,&hints,&res))!=0)
		err_quit("udp_server error for %s,%s:%s",host,serv,gai_strerror(n));

	ressave=res;

	do{
		sockfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);

		if(sockfd<0)
			continue;
		if(bind(sockfd,res->ai_addr,res->ai_addrlen)==0)
			break;

		if(close(sockfd)<0)
			err_sys("close");

	}while((res=res->ai_next)!=NULL);

	if(res==NULL)
		err_sys("udp_server error for %s,%s",host,serv);

	if(addrlenp)
		*addrlenp=res->ai_addrlen;

	freeaddrinfo(ressave);

	return sockfd;
}

extern int daemon_proc;

int daemon_init(const char *pname,int facility)
{
	int i;
	pid_t pid;
	
	if((pid=fork())<0)
		return -1;
	else if(pid)
		_exit(0);
	if(setsid()<0)
		return -1;
	
	if(signal(SIGHUP,SIG_IGN)==SIG_ERR)
		err_sys("signal");

	if((pid=fork())<0)
		return -1;
	else if(pid)
		_exit(0);

	daemon_proc=1;
	
	chdir("/");

	for(i=0;i<MAXFD;i++)
		close(i);

	open("/dev/NULL",O_RDONLY);
	open("/dev/NULL",O_RDWR);
	open("/dev/NULL",O_RDWR);

	openlog(pname,LOG_PID,facility);

	return 0;

}

static void connect_alarm(int);

int connect_timeo(int sockfd,const SA *saptr,socklen_t salen,int nsec)
{
	sigfunc *sigfunc;

	int n;
	if((sigfunc=signal(SIGALRM,connect_alarm))==SIG_ERR)
		err_sys("signal");

	if(alarm(nsec)!=0)
		err_msg("connect_timeo:alarm wast already set");

	if((n=connect(sockfd,saptr,salen))<0)
	{
		close(sockfd);
		if(errno==EINTR)
			errno=ETIMEDOUT;
	}
	alarm(0);
	if(signal(SIGALRM,sigfunc)==SIG_ERR)
		err_sys("signal");

	return (n);
}
static void connect_alarm(int signo)
{
	return;
}
void daemon_inetd(const char *pname,int facility)
{
	daemon_proc=1;
	openlog(pname,LOG_PID,facility);
}
char *gf_time(void)
{
	struct timeval tv;
	time_t t;
	static char str[30];
	char *ptr;

	if(gettimeofday(&tv,NULL)<0)
			err_sys("gettimeofday error");
	t=tv.tv_sec;
	
	ptr=ctime(&t);
	strcpy(str,&ptr[11]);
	snprintf(str+8,sizeof(str)-8,".%06ld",tv.tv_usec);
	return str;
}
