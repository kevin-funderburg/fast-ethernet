//
// Created by Kevin Funderburg on 12/10/20.
//

#ifndef TCP_H
#define TCP_H

#endif //TCP_H
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
//#include <pthread.h>
#define LISTENQ 5
#define SERV_PORT 9877
/* Following shortens all the typecasts of pointer arguments: */
#define	SA	struct sockaddr

int		        Accept(int, SA *, socklen_t *);
void	        Bind(int, const SA *, socklen_t);
void            Connect(int, const SA *, socklen_t);
void            err_sys(const char* msg);
void            err_quit(const char* msg);
int             Socket(int family, int type, int protocol);
const char*     Inet_ntop(int family, const void *addrptr, char *strptr, size_t len);
void            Writen(int fd, void *ptr, size_t nbytes);
ssize_t         writen(int fd, const void *vptr, size_t n);


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