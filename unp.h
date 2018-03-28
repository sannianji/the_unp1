#ifndef __UNP_H
#define __UNP_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#define CPU_VENDOR_OS "x86_64-unknown-linux-gnu"
#define SA struct sockaddr
#define MAXLINE 1024
#define LISTENQ 1024
#define SERV_PORT 9877
char *sock_ntop(const struct sockaddr*,socklen_t);
ssize_t writen(int fd,const void *vptr,size_t n);
ssize_t readn(int,void*,size_t);
ssize_t readline(int,void *,size_t);
ssize_t readline2(int,void *,size_t);
ssize_t readlinbuf(void **);
void err_sys(const char *,...);
void err_quit(const char *,...);
int sockfd_to_family(int);

#endif