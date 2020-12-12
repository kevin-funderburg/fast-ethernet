#include "csp.h"
#include "tcp.h"

FrameQueue* dataQ;
FrameQueue* reqQ;
int         stationCount;
int		    nready, client[FD_SETSIZE];
fd_set	    wset, rset, allset;       //set of file descriptors
int         stationNums[10];
int         instCount;
bool        queuesSet = false;

int logger(frame* f)
{
    printf("\n...logger()...\n");
    if (f == NULL) return -1;

    char buf[MAXCHAR];
    char filename[MAXLINE];
    snprintf(filename, MAXLINE, "src/output/csp.txt");
    FILE *fp = fopen(filename, "a");
    if(!fp){
        printf("error: unable to read source file %s\n", filename); 
        err_sys("couldn't open input file\n");
    }
    char msg[MAXLINE];
    instCount++;
    switch (f->type) {
        case REQUEST: {
            snprintf(msg, MAXLINE, "%d) Receive request from SP %d\n", instCount, f->src);
            break;
        } case DATA: {
            snprintf(msg, MAXLINE, "%d) Receive data frame from %d to SP %d\n", instCount, f->src, f->dest);
            break;
        } case POS_REPLY: {
            snprintf(msg, MAXLINE, "%d) Send positive reply to SP %d\n", instCount, f->dest);
            break;
        } case NEG_REPLY: {
            snprintf(msg, MAXLINE, "%d) Send positive reply to SP %d\n", instCount, f->dest);
            break;
        } case FORWARD: {
            snprintf(msg, MAXLINE, "%d) Forward data frame (from SP %d) to SP %d\n", instCount, f->src, f->dest);
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


/**
 * parse the input from the station processes
 * @param argv
 * @return int - reply status
 */
frame * argparse(char* argv)
{
    printf("argv: %s\n", argv);
    if (strcmp(argv, "") != 0)
    {
        frame* incoming = textToFrame(argv);
        return incoming;
    }
}


/**
 * reply to request sent from station runner
 * @param argv
 * @return int - reply status
 */
frame* reply(frame *incoming)
{
//    frame* f = textToFrame(argv);
    frame* response;
    if (isFull(dataQ)) {
        enqueue(reqQ, incoming);
        printf("data queue full, need to send a wait signal\n");
        response = newFrame(currentSequence++, CSP_ADDR, incoming->src, WAIT_MSG, NEG_REPLY);
    } else {
        printf("data not full, send signal that it can send a frame\n");
        response = newFrame(currentSequence++, CSP_ADDR, incoming->src, SEND_MSG, NEG_REPLY);
    }
    return response;
}


int sendToSP(frame *f)
{
    printf("...sendToSP()...\n");
    char buf[MAXLINE];
    bzero(buf, MAXLINE);

    printf("f->dest: %d\n", f->dest);
    printf("==>BREAK==>\n");
    int fdIndex = getDescriptor(f->dest);
    printf("fdIndex: %d\n", fdIndex);
    char* msg;
    if (fdIndex != -1) {
        msg = frameToText(f);
        strcpy(buf, msg);
        Writen(fdIndex, buf, MAXLINE);
    }
    printf("\n...done | sendToSP()...\n");
}


/**
 * get the index of the desired station runner's descriptor
 * from the descriptor array
 * @param stationNum
 * @return int
 */
int getDescriptor(int stationNum) { return client[stationNum]; }


/**
 * we will assume for now that the station will send the csp 'frame' that looks like:
 * "sequence,srcaddress,destinationaddr,data"
 */
int main(int argc, char **argv)
{
    int					i, maxi, maxfd, listenfd, connfd, sockfd;
    ssize_t				n;
    int                 signal;
    char*               reply;
    socklen_t			clilen;
    char				buf[MAXLINE];
    struct sockaddr_in	cliaddr, servaddr;
    instCount = 0;

    if (!queuesSet) {
        currentSequence = 0;
        stationCount = 0;
        dataQ = newQueue();
        reqQ = newQueue();
        queuesSet = true;
    }
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    printf("listening...\n");
    maxfd = listenfd;			/* initialize */
    maxi = -1;					/* index into client[] array */
    for (i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;			/* -1 indicates available entry */
    FD_ZERO(&allset);           // clear all bits in fdset
    FD_SET(listenfd, &allset);  // turn on the bit for fd in fdset

    for ( ; ; ) {
        rset = allset;		/* structure assignment */
        wset = allset;
        nready = Select(maxfd+1, &rset, &wset, NULL, NULL);
//        nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &rset))  // new client connection
        {
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
            printf("new client added: %s, port %d\n", "fix later", ntohs(cliaddr.sin_port));

            for (i = 0; i < FD_SETSIZE; i++)
                if (client[i] < 0) {
                    printf("inserting into connfd (%d) into client[%d]: %d\n", connfd, i);
                    client[i] = connfd;	/* save descriptor */
                    stationNums[i] = ntohs(cliaddr.sin_port);
                    printf("inserting portnum (%d) into stationNums[%d]: %d\n", ntohs(cliaddr.sin_port), i);
                    printf("done\n");
                    stationCount++;
                    printf("stationCount: %d\n", stationCount);
                    break;
                }
            if (i == FD_SETSIZE)
                err_quit("too many clients");

            FD_SET(connfd, &allset);	/* add new descriptor to set */
            if (connfd > maxfd)
                maxfd = connfd;			/* for select */
            if (i > maxi)
                maxi = i;				/* max index in client[] array */

            if (--nready <= 0)
                continue;				/* no more readable descriptors */
        }

        for (i = 0; i <= maxi; i++)         // check all stations for data
        {
            bzero(buf, MAXLINE);
            sockfd = client[i];
            if ( (sockfd) < 0)
                continue;
            if (FD_ISSET(sockfd, &rset))
            {
                if ( (n = read(sockfd, buf, MAXLINE)) == 0) {
                    /*4connection closed by client */
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                    printf("client connection closed\n");
                }
                else
                {
                    printf("sockfd: %d\n", sockfd);
                    printf("SIZEOF n: %zd\n", n);
                    frame* incoming = argparse(buf);
                    frame* response;
                    switch (incoming->type)
                    {

                        case DATA:
                            response = newFrame(currentSequence++, incoming->src, sockfd, "wait", FORWARD);
                            enqueue(dataQ, incoming);
                            break;
                        case REQUEST:
                        {
                            enqueue(reqQ, incoming);
                            if (isFull(dataQ))
                                response = newFrame(currentSequence++, incoming->src, sockfd, "wait", NEG_REPLY);
                            else
                                response = newFrame(currentSequence++, incoming->src, sockfd, "send", POS_REPLY);
                            char* txt;
                            txt = frameToText(response);
                            bzero(buf, sizeof(buf));
                            printf("txt: %s\n", txt);
                            strcpy(buf, txt);
                            printf("SENDING: %s\n", buf);
                            write(sockfd, buf, sizeof(buf));
                            break;
                        }
                        default:
                            break;
                    }

                    logger(response);

                    char* txt;

                    char buf[MAXLINE];
                    printf("\tdataQ->size: %d\n", dataQ->size);
                    int q = dataQ->size;
                    for (int i = 0; i < q; ++i)
                    {
                        printf("i: %d\n", i);
                        bzero(buf, MAXLINE);
                        frame *t = top(dataQ);
                        txt = frameToText(t);
                        printf("txt: %s\n", txt);
                        strcpy(buf, txt);
                        printf("SENDING: %s\n", buf);
                        write(sockfd, buf, sizeof(buf));

                        bzero(buf, sizeof(buf));
                        read(sockfd, buf, sizeof(buf));

                        //strcpy(buf, reply);
                        //Writen(sockfd, buf, MAXLINE);
                        printf("==>BREAK==>\n");
                        pop(dataQ);
                    }
                }

                if (--nready <= 0)
                    break;				/* no more readable descriptors */
            }
        }
    }
}
