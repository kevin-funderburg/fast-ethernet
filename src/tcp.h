//
// Created by Kevin Funderburg on 12/10/20.
//

#ifndef TCP_H
#define TCP_H

#include    <sys/types.h>   /* basic system data types */
#include    <sys/socket.h>  /* basic socket definitions */
#if TIME_WITH_SYS_TIME
#include    <sys/time.h>    /* timeval{} for select() */
#include    <time.h>        /* timespec{} for pselect() */
#else
#if HAVE_SYS_TIME_H
#include    <sys/time.h>    /* includes <time.h> unsafely */
#else
#include    <time.h>        /* old system? */
#endif
#endif
#include    <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include    <arpa/inet.h>   /* inet(3) functions */
#include    <errno.h>
#include    <fcntl.h>       /* for nonblocking */
#include    <netdb.h>
#include    <signal.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <sys/stat.h>    /* for S_xxx file mode constants */
#include    <sys/uio.h>     /* for iovec{} and readv/writev */
#include    <unistd.h>
#include    <sys/wait.h>
#include    <sys/un.h>      /* for Unix domain sockets */
#include    <sys/select.h>
#include    <stdbool.h>
#include    "frame.h"

#define LISTENQ 5
#define MAXLINE 4096
#define MAXCHAR 1024
#define SERV_PORT 9877
#define INPUT "input.txt"

int currentSequence;
int stationCount;

/* Following shortens all the typecasts of pointer arguments: */
#define	SA	struct sockaddr

int		        Accept(int, SA *, socklen_t *);
void	        Bind(int, const SA *, socklen_t);
void            Connect(int, const SA *, socklen_t);
void            err_sys(const char* msg);
void            err_quit(const char* msg);
int             Socket(int family, int type, int protocol);
void            Shutdown(int fd, int how);
const char*     Inet_ntop(int family, const void *addrptr, char *strptr, size_t len);
void            Writen(int fd, void *ptr, size_t nbytes);
ssize_t         writen(int fd, const void *vptr, size_t n);


enum SIGNAL {REJECT, SEND, ADDED, INVALID, NODEST, STATION_NUM};


int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int		n;

    again:
    if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef	EPROTO
        if (errno == EPROTO || errno == ECONNABORTED)
#else
        if (errno == ECONNABORTED)
#endif
            goto again;
        else
            err_sys("accept error");
    }
    return(n);
}


void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (bind(fd, sa, salen) < 0)
        err_sys("bind error");
}


int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout)
{
    int		n;

    if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
        err_sys("select error");
    return(n);		/* can return 0 on timeout */
}


int Socket(int family, int type, int protocol)
{
    int		n;

    if ( (n = socket(family, type, protocol)) < 0)
        err_sys("socket error");
    return(n);
}



const char * Inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
    const char	*ptr;

    if (strptr == NULL)		/* check for old code */
        err_quit("NULL 3rd argument to inet_ntop");
    if ( (ptr = inet_ntop(family, addrptr, strptr, len)) == NULL)
        err_sys("inet_ntop error");		/* sets errno */
    return(ptr);
}


/* Fatal error related to system call
 * Print message and terminate */

void err_sys(const char *msg)
{
    perror(msg);
    exit(1);
}

void err_quit(const char *msg)
{
    perror(msg);
    exit(1);
}


void Shutdown(int fd, int how)
{
    if (shutdown(fd, how) < 0)
        err_sys("shutdown error");
}


ssize_t writen(int fd, const void *vptr, size_t n)		/* Write "n" bytes to a descriptor. */
{
    size_t		nleft;
    ssize_t		nwritten;
    const char	*ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;		/* and call write() again */
            else
                return(-1);			/* error */
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n);
}
/* end writen */

void Writen(int fd, void *ptr, size_t nbytes)
{
    if (writen(fd, ptr, nbytes) != nbytes)
        err_sys("writen error");
}


/* macros */
#define	min(a,b)	((a) < (b) ? (a) : (b))
#define	max(a,b)	((a) > (b) ? (a) : (b))

#endif //TCP_H
