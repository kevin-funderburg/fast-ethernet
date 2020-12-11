//
// Created by Kevin Funderburg on 12/10/20.
//

#include "station.h"
FrameQueue* waitingQ;
int currentSequence;
int stationNum;

void logger()
{
    //TODO
}

char* getInput()
{
    char buf[MAXCHAR];
    FILE *fp = fopen("src/data.txt", "r");
    if(!fp){
        printf("error: unable to read source file %s\n", INPUT);
        err_sys("couldn't open input file\n");
    }
    while (fgets(buf, MAXCHAR, fp)) {
        printf("buf: %s\n", buf);
        parseLine(buf);
    }
    fclose(fp);
}


char* parseLine(char* line)
{
    if ((strncmp("Frame", line, 5) == 0))
    {
        printf("Its a frame request\n");
        char *end;
        const char *seps = " \t\r\n\f\v,";
        char *p = strtok_r(line, seps, &end);
        printf("%s\n", p);
        int i = 0;
        char *items[5];
        int inst;
        char* msg;
        int destFrameNum, sendFrameNum, waitFrameNum;
        while ((p = strtok_r(NULL, seps, &end)))
        {
            if (i == 0) {
                if ((strcmp(p, "Frame") == 0)) {
                    printf("this is a request to send\n");
                    inst = SEND_INST;
                } else if ((strcmp(p, "Wait") == 0)) {
                    printf("this is a request to send\n");
                    inst = WAIT_INSTR;
                }
            } else
            {
                if (inst == SEND_INST) {
                    if (i == 1) sendFrameNum = atoi(p);
                    else if (i == 4) destFrameNum = atoi(p);
                } else
                    if (i == 4) waitFrameNum = atoi(p);
            }
            i++;
        }
        if (inst == SEND_INST) {
            frame* f = newFrame(currentSequence++, stationNum, destFrameNum, "some data");

        }
    }
}

void str_cli(FILE *fp, int sockfd) {
    int			maxfdp1, stdineof;
    fd_set		rset;
    char		buf[MAXLINE];
    int		    n;


    stdineof = 0;           // as long as 0, select on standard in for read
    FD_ZERO(&rset);     // initialize the descriptor set
    for ( ; ; ) {
        bzero(buf, MAXLINE);

        if (stdineof == 0)
            FD_SET(fileno(fp), &rset);   // turn on the IO file pointer bit
        FD_SET(sockfd, &rset);              //  turn on the corresponding socket bit

        // fileno() changes fp pointer to descriptor
        // select will select the max of the desriptors
        maxfdp1 = max(fileno(fp), sockfd) + 1;
        Select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {	/* socket is readable */
            if ((n = read(sockfd, buf, MAXLINE)) == 0) {    //read from cps
                if (stdineof == 1)
                    return;        /* normal termination */
                else
                    err_quit("station: server terminated prematurely");
            }
            printf("RECEIVING: %s\n", buf);

            if ((strncmp("send", buf, 4) == 0))
            {
                bzero(buf, MAXLINE);
                strcpy(buf, "1,sdfw,lskd,asdf");
                printf("SENDING FRAME: %s\n", buf);
                Writen(sockfd, buf, sizeof(buf));
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
            printf("SENDING: %s\n", buf);
            Writen(sockfd, buf, n);     //send to cps
        }
    }
}

int main(int argc, char **argv)
{
    int					sockfd;
    struct sockaddr_in	servaddr;
    waitingQ = newQueue();
    stationNum = -1;

    printf("argv: %s\n", argv);
    if (argc != 2)
        err_sys("usage: station <IPaddress>");
    if (strcmp(argv[1], "test") == 0) {
        getInput();
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);		/* do it all */

    exit(0);
}
