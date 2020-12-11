//
// Created by Kevin Funderburg on 12/10/20.
//

#include "station.h"
FrameQueue* waitingQ;
int stationNum;
int					sockfd;

void testRunner()
{
    printf("...testRunner()...\n");
    char* txt = "1,1,3,some data";
    frame* f = textToFrame(txt);
    printf("f: %d,%d,%d,%s,%d\n", f->seq, f->src, f->dest, f->data, f->type);
    printf("it worked\n");
}

void logger()
{
    //TODO
}

void getInput()
{
    char buf[MAXCHAR];
    FILE *fp = fopen("src/input/1.txt", "r");
    if(!fp){
        printf("error: unable to read source file %s\n", "src/input/1.txt");
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

        sendRequest();

        char *end;
        const char *seps = " \t\r\n\f\v,";
        char *p = strtok_r(line, seps, &end);
        printf("p: %s\n", p);
        int i = 0;
        int inst;
        int destFrameNum, sendFrameNum, waitFrameNum;
        printf("inside loop\n");
        while ((p = strtok_r(NULL, seps, &end)))
        {
            printf("p: %s\n", p);
            if (i == 0)
                sendFrameNum = atoi(p);
            else if (i == 3)
                destFrameNum = atoi(p);
            i++;
        }
//        printf("currentSequence: %d\n", currentSequence);
//        printf("sendFrameNum: %d\n", sendFrameNum);
//        printf("destFrameNum: %d\n", destFrameNum);
        frame* f = newFrame(currentSequence++, sendFrameNum, destFrameNum, "some data", DATA);
        enqueue(waitingQ, f);
//        printf("waitingQ size: %d\n", waitingQ->size);

    } else if ((strncmp("Wait", line, 5) == 0))
    {
        printf("this is a wait instruction\n");
        char *end;
        const char *seps = " \t\r\n\f\v,";
        char *p = strtok_r(line, seps, &end);
        printf("p: %s\n", p);
        int i = 0;
        int inst;
        int destFrameNum, sendFrameNum, waitFrameNum;
//        printf("inside loop\n");
        while ((p = strtok_r(NULL, seps, &end)))
        {
            printf("p: %s\n", p);
            if (i == 4)
                waitFrameNum = atoi(p);
            i++;
        }
        frame* f = newFrame(currentSequence++, -1, destFrameNum, "", WAIT_INSTR);
        enqueue(waitingQ, f);
    }
}


int sendRequest()
{
    currentSequence++;
    char buf[MAXLINE];
    bzero(buf,  sizeof(buf));
    char* msg = "request";
    strcpy(buf, msg);
    printf("%d) SENDING REQUEST: %s\n", currentSequence, buf);
    Writen(sockfd, buf, sizeof(buf));
}


void sendFrame()
{
    printf("...sendFrame()...\n");
    char* buf[MAXLINE];
    frame* front = top(waitingQ);
//    printf("front->seq: %d\n", front->seq);
    char* msg = frameToText(front);
    printf("msg: %s\n", msg);
    strcpy(buf, msg);
    printf("SENDING FRAME: %s\n", buf);
    Writen(sockfd, buf, sizeof(buf));
    printf("...done...\n");
}

//void getStationNum()




void runner(FILE *fp) {
    int			maxfdp1, stdineof;
    fd_set		rset;
    char		buf[MAXLINE];
    int		    n;

//    stationNum = stationCount;
//    printf("stationNum: %d\n", stationNum);

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
                sendFrame();
            }
            else if ((strncmp("wait", buf, 4) == 0))
            {
//                for (int i = 0; i < ; ++i) {
//
//                }
            }

//            write(fileno(stdout), buf, n);
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
    struct sockaddr_in	servaddr;
    waitingQ = newQueue();
    stationNum = none;

    getInput();
//    printf("argv: %s\n", argv);
    if (argc != 2)
        err_sys("usage: station <IPaddress>");
    if (strcmp(argv[1], "test") == 0) {
//        getInput();
//        printQueue(waitingQ);
        testRunner();
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("sockfd: %d\n", sockfd);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);

    printf("servaddr.sin_port: %d\n", servaddr.sin_port);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    runner(stdin);		/* do it all */

    exit(0);
}
