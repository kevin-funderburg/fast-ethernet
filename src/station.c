//
// Created by Kevin Funderburg on 12/10/20.
//

#include "tcp.h"
#include "frame.h"

void logger()
{
    //TODO
}

char* getInst()
{
    FILE *input;
    input = fopen(input, "r");
    if(!input){
        printf("error: unable to read source file %s\n", INPUT);
        err_sys("couldn't open input file\n");
    }

}
void str_cli(FILE *fp, int sockfd)
{
    int			maxfdp1, stdineof;
    fd_set		rset;
    char		buf[MAXLINE];
    int		    n;

    stdineof = 0;           // as long as 0, select on standard in for read
    FD_ZERO(&rset);     // initialize the descriptor set
    for ( ; ; ) {
        if (stdineof == 0)
            FD_SET(fileno(fp), &rset);   // turn on the IO file pointer bit
        FD_SET(sockfd, &rset);              //  turn on the corresponding socket bit

        // fileno() changes fp pointer to descriptor
        // select will select the max of the desriptors
        maxfdp1 = max(fileno(fp), sockfd) + 1;
        Select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {	/* socket is readable */
            if ((n = read(sockfd, buf, MAXLINE)) == 0) {
                if (stdineof == 1)
                    return;        /* normal termination */
                else
                    err_quit("str_cli: server terminated prematurely");
            }
            write(fileno(stdout), buf, n);
        }

        if (FD_ISSET(fileno(fp), &rset)) {  /* input is readable */
            if ( (n = read(fileno(fp), buf, MAXLINE)) == 0) {
                stdineof = 1;
                Shutdown(sockfd, SHUT_WR);	/* send FIN */
                FD_CLR(fileno(fp), &rset);
                continue;
            }

            Writen(sockfd, buf, n);
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
