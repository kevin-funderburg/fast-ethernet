#include "csp.h"
#include "tcp.h"

FrameQueue* dataQ;
FrameQueue* reqQ;
int         stationCount;
int		    nready, client[FD_SETSIZE];
fd_set	    wset, rset, allset;       //set of file descriptors
int         stationNums[10];
bool        queuesSet = false;


void getInstr(char* line)
{

}


/**
 * parse the input from the station processes
 * @param argv
 * @return int - reply status
 */
int argparse(char* argv)
{

//    if (strcmp)
    frame* incoming = textToFrame(argv);
    enqueue(reqQ, incoming);

    printf("...argpase()...\n"
           "argv: %s\n", argv);

//    char args[10][25];
//    char *pch;
//    int argc = 0;
//    char* buf[MAXLINE];

    if (incoming->type == REQUEST)
    {
        frame* replyFrame = reply(incoming);
        sendToSP(replyFrame);
    }

//    if ((strncmp("request", argv, 7) == 0))
//    {
//        printf("\nit's a request.\n");
//        return reply(argv);
//    }
//    else if ((strncmp("getSP", argv, 5) == 0))
//    {
//        printf("getting client: %s\n", argv);
//        char* pch;
//        pch = strtok(argv, " ");
//        strcpy(args[0], pch);
//        int clientindex = -1;
//
//        while (pch != NULL)
//        {
//            argc++;
//            clientindex = atoi(pch);
//            pch = strtok(NULL, " ,.-");
//            if (pch != NULL) strcpy(args[argc], pch);
//        }
//        int fdIndex = getDescriptor(clientindex);
//        char* msg;
//        if (fdIndex != -1)
//        {
//            msg = "calling from other worlds\n";
//            strcpy(buf, msg);
//            Writen(fdIndex, buf, MAXLINE);
//        }
//        else {
//            msg = "SP not connected\n";
//            return NODEST;
//        }
//
//    }
//    else
//    {
//        printf("it's a frame.\n");
//        return processFrame(argv);
//    }
}


int processFrame(char* argv)
{
    char args[10][25];
    char *pch;
    int argc = 0;
    char* buf[MAXLINE];

    pch = strtok(argv, ",");
    strcpy(args[0], pch);
    while (pch != NULL)
    {
        argc++;
        pch = strtok(NULL, " ,.-");
        if (pch != NULL)
//                printf("pch: %s\n", pch);
            strcpy(args[argc], pch);
    }

    printf("parsed args:\n");
    for (int i = 0; i < argc; ++i)
        printf("args[%d]\t%s\n", i, args[i]);

    if (argc == 4) {
        frame* f = newFrame(atoi(args[0]), args[1], args[2], args[3], DATA);
        enqueue(dataQ, f);
        printf("new frame added\n");
        printf("dataQ size: %d\n", dataQ->size);
        return ADDED;
    } else {
        printf("argv is not a valid format: %s\n", argv);
        return INVALID;
    }
}


/**
 * reply to request sent from station runner
 * @param argv
 * @return int - reply status
 */
frame * reply(frame *incoming)
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
    char buf[MAXLINE];
    int fdIndex = getDescriptor(f->dest);
    char* msg;
    if (fdIndex != -1) {
        msg = frameToText(send);
        strcpy(buf, msg);
        Writen(fdIndex, buf, MAXLINE);
    }
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


                    signal = argparse(buf);
                    switch (signal) {
                        case SEND: reply = "send it\n"; break;
                        case REJECT: reply = "wait\n"; break;
                        case ADDED: reply = "frame added\n"; break;
                        case INVALID: reply = "invalid string\n"; break;
                        case NODEST: reply = "SP not connected\n"; break;
                        case STATION_NUM: {
                            char reply[MAXLINE];
                            snprintf(reply, MAXLINE, "your station number: %d\n", stationCount);
                        }
                        default: reply = "unknown error\n"; break;
                    }

                    strcpy(buf, reply);
                    Writen(sockfd, buf, MAXLINE);
                }

                if (--nready <= 0)
                    break;				/* no more readable descriptors */
            }
        }
    }
}
