#ifndef __UNP_H
#define __UNP_H
#include <syslog.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <poll.h>
#include <sys/stropts.h>
#include <limits.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#define CPU_VENDOR_OS "x86_64-unknown-linux-gnu"
#define SA struct sockaddr
#define MAXLINE 1024
#define LISTENQ 1024
#define SERV_PORT 9877
#define INFTIM -1
#define OPEN_MAX 1024
#define MAXFD 64
typedef void sigfunc(int);
int daemon_proc;
char *sock_ntop(const struct sockaddr*,socklen_t);
ssize_t writen(int fd,const void *vptr,size_t n);
ssize_t readn(int,void*,size_t);
ssize_t readline(int,void *,size_t);
ssize_t readline2(int,void *,size_t);
ssize_t readlinbuf(void **);
void err_sys(const char *,...);
void err_quit(const char *,...);
void err_msg(const char *,...);
void err_ret(const char *,...);
int sockfd_to_family(int);
int maxfd(int,int);
int minfd(int,int);
<<<<<<< HEAD
struct addrinfo* host_serv(const char *host,const char *serv,int family,int socktype);
=======
struct addrinfo *host_serv(const char *host,const char *serv,int family,int socktype);
int tcp_connect(const char *host,const char *serv);
int tcp_listen(const char *host,const char *serv,socklen_t *addrlenp);
int udp_client(const char *host,const char *serv,SA **saptr,socklen_t *lenp);
int udp_connect(const char *host,const char *serv);
int udp_server(const char *host,const char *serv,socklen_t *addrlenp);
int daemon_init(const char *pname,int facility);
int connect_timeo(int sockfd,const SA *saptr,socklen_t salen,int nsec);
int daemon_inetd(const char *,int);
char* gf_time(void);
>>>>>>> d03392ff4e44c4e1953ea09d65c5efb1295b79e8
#endif
