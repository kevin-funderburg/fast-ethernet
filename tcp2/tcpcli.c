//
// Created by Kevin Funderburg on 12/10/20.
//

#include "tcp.h"


void str_cli(FILE *fp, int sockfd)
{
    int			maxfdp1;
    fd_set		rset;
    char		sendline[MAXLINE], recvline[MAXLINE];

    FD_ZERO(&rset);     // initialize the descriptor set
    for ( ; ; ) {
        FD_SET(fileno(fp), &rset);   // turn on the IO file pointer bit
        FD_SET(sockfd, &rset);          //  turn on the corresponding socket bit
        // fileno() changes fp pointer to descriptor
        // select will select the max of the desriptors
        maxfdp1 = max(fileno(fp), sockfd) + 1;
        Select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {	/* socket is readable */
            if (read(sockfd, recvline, MAXLINE) == 0)
//            if (readline(sockfd, recvline, MAXLINE) == 0)
                err_quit("str_cli: server terminated prematurely");
            fputs(recvline, stdout);
        }

        if (FD_ISSET(fileno(fp), &rset)) {  /* input is readable */
            if (fgets(sendline, MAXLINE, fp) == NULL)
                return;		/* all done */
            Writen(sockfd, sendline, strlen(sendline));
        }
    }
}

int main(int argc, char **argv)
{
    int					sockfd;
    struct sockaddr_in	servaddr;

    if (argc != 2)
        err_sys("usage: tcpcli <IPaddress>");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);		/* do it all */

    exit(0);
}
