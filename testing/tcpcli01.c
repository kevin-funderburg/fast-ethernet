//#include	"./lib/str_echo.c"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXLINE 1000
#define SERV_PORT 9877

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void
str_cli(FILE *fp, int sockfd)
{
    char	sendline[MAXLINE], recvline[MAXLINE];

    while (fgets(sendline, MAXLINE, fp) != NULL) {

        write(sockfd, sendline, strlen(sendline));

        if (read(sockfd, recvline, MAXLINE) == 0)
            error("str_cli: server terminated prematurely");

        fputs(recvline, stdout);
    }
}

int
main(int argc, char **argv)
{
    int					sockfd;
    struct sockaddr_in	servaddr;

    if (argc != 2)
        error("usage: tcpcli <IPaddress>");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);		/* do it all */

    exit(0);
}
