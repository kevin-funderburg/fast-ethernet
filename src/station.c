//
// Created by Kevin Funderburg on 12/10/20.
//

#include "station.h"
FrameQueue* waitingQ;
int         stationNum;
int			sockfd;
int         instCount;

void testRunner()
{
    printf("\n...testRunner()...\n");
    char buf[MAXLINE];
    char* txt;
    printf("\twaitingQ->size: %d\n", waitingQ->size);
    int q = waitingQ->size;
    for (int i = 0; i < q; ++i) {
        printf("i: %d\n", i);
        bzero(buf, MAXLINE);
        frame* t = top(waitingQ);
        txt = frameToText(t);
        printf("txt: %s\n", txt);
        strcpy(buf, txt);
//        Writen(sockfd, buf, n);     //send to cps
        pop(waitingQ);
    }
    printf("\n...done | testRunner()...\n");
}

int logger(frame* f)
{
    printf("\n...logger()...\n");
    if (f == NULL) return -1;

    char buf[MAXCHAR];
    char filename[MAXLINE];
    snprintf(filename, MAXLINE, "src/output/%d.txt", stationNum);
    FILE *fp = fopen(filename, "a");
    if(!fp){
        printf("error: unable to read source file %s\n", filename);
        err_sys("couldn't open input file\n");
    }
    char msg[MAXLINE];
    instCount++;
    switch (f->type) {
        case REQUEST: {
            snprintf(msg, MAXLINE, "%d) Send request to CSP to send data frame %d to SP %d\n", instCount, f->src, f->dest);
            break;
        } case DATA: {
            snprintf(msg, MAXLINE, "%d) Send (via CSP) data frame %d to SP %d\n", instCount, f->dest);
            break;
        } case POS_REPLY: {
            snprintf(msg, MAXLINE, "%d) Receive positive reply (permission) from CSP to send data frame %d to SP %d\n", instCount, f->dest);
            break;
        } case NEG_REPLY: {
            snprintf(msg, MAXLINE, "%d) Receive reject reply from CSP to send data frame %d to SP %d\n", instCount, f->dest);
            break;
        } default:
            snprintf(msg, MAXLINE, "unknown message type: %d\n", f->type);
            break;
    }
    printf("msg: %s\n", msg);
    fputs(msg, fp);
    printf("\n...done | logger()...\n");
    return 0;
}

void getInput()
{
    printf("\n...getInput()...%\n");
    char buf[MAXCHAR];
    char filename[MAXLINE];
    snprintf(filename, MAXLINE, "src/input/%d.txt", stationNum);
    printf("filename: %s\n", filename);
    FILE *fp = fopen(filename, "r");
    if(!fp){
        printf("error: unable to read source file %s\n", "src/input/1.txt");
        err_sys("couldn't open input file\n");
    }
    while (fgets(buf, MAXCHAR, fp)) {
        printf("buf: %s\n", buf);
        parseLine(buf);
    }
    fclose(fp);
    printf("\n...done | getInput()...%\n");

}


char* parseLine(char* line)
{
    printf("\n...parseLine(char* line)...\n");
    if ((strncmp("Frame", line, 5) == 0))
    {
        printf("Its a frame request\n");

//        sendRequest();

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
        frame* f = newFrame(currentSequence++, stationNum, destFrameNum, "", WAIT_INSTR);
        enqueue(waitingQ, f);
    }
    printf("\n...done | parseLine()...%\n");

}


int sendRequest()
{
    printf("\n...sendRequest()...%\n");
    frame* request = newFrame(currentSequence++, stationNum, CSP_ADDR, "request", REQUEST);
    char buf[MAXLINE];
    bzero(buf,  sizeof(buf));
    char* msg = frameToText(request);
    strcpy(buf, msg);
    printf("%d) SENDING REQUEST: %s\n", currentSequence, buf);
    Writen(sockfd, buf, sizeof(buf));
    printf("\n...done | sendRequest()...%\n");

}


void sendFrame()
{
    printf("\n...sendFrame()...\n");
    char* buf[MAXLINE];
    bzero(buf, MAXLINE);
    frame* front = top(waitingQ);
//    printf("front->seq: %d\n", front->seq);
    char* msg = frameToText(front);
    printf("msg: %s\n", msg);
    strcpy(buf, msg);
    printf("SENDING FRAME: %s\n", buf);
    Writen(sockfd, buf, sizeof(buf));
    printf("\n...done | sendFrame()...\n");
}

//void getStationNum()




void runner(FILE *fp) {
    printf("\n...runner(FILE *fp)...\n");
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
            if ((strcmp(buf, "") != 0)) {
                frame *received = textToFrame(buf);
                logger(received);
            }
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
            char* txt;

            char buf[MAXLINE];
            printf("\twaitingQ->size: %d\n", waitingQ->size);
            int q = waitingQ->size;
            for (int i = 0; i < q; ++i) {
                printf("i: %d\n", i);
                bzero(buf, MAXLINE);
                frame* t = top(waitingQ);
                txt = frameToText(t);
                printf("txt: %s\n", txt);
                strcpy(buf, txt);
                printf("SENDING: %s\n", buf);
                logger(t);
//                Writen(sockfd, buf, n);     //send to cps
                write(sockfd, buf, sizeof(buf));
                bzero(buf, sizeof(buf));
                read(sockfd, buf, sizeof(buf));
                printf("RECEIVING: %s\n", buf);
                if ((strcmp(buf, "") != 0)) {
                    frame *received = textToFrame(buf);
                    logger(received);
                }
//                if ( (n = read(fileno(fp), buf, MAXLINE)) == 0) {
//                    stdineof = 1;
//                    Shutdown(sockfd, SHUT_WR);	/* send FIN */
//                    FD_CLR(fileno(fp), &rset);
//                    continue;
//                }
                pop(waitingQ);
            }
        }
    printf("\n...done | runner(FILE *fp)...\n");
    }

}

int main(int argc, char **argv)
{
    struct sockaddr_in	servaddr;
    waitingQ = newQueue();
    stationNum = none;
    instCount = 0;
    printf("argc: %d\n", argc);
    getInput();

    if (argc < 2)
        err_sys("usage: station <[0-9]>");
    if (argc == 2) {
        if (strcmp(argv[1], "test") == 0) {
            testRunner();
            exit(0);
        } else {
            printf("its 2\n");
            printf("argv[1]: %s\n", argv[1]);
            stationNum = atoi(argv[1]);
        }
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
