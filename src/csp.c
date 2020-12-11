#include "csp.h"
#include "tcp.h"

FrameQueue* dataQ;
FrameQueue* reqQ;
int         stationCount;


bool        queuesSet = false;

int					nready, client[FD_SETSIZE];
fd_set				wset, rset, allset;       //set of file descriptors


void getInstr(char* line)
{

}

int argparse(char* argv)
{
    printf("...argpase()...\n"
           "argv: %s\n", argv);
    char args[10][25];
    char *pch;
    int argc = 0;
    char* buf[MAXLINE];

    if ((strncmp("request", argv, 7) == 0))
    {
        printf("\nit's a request.\n");
        return reply()
    }
    else if ((strncmp("getSP", argv, 5) == 0))
    {
        printf("getting child: %s\n", argv);
        char* pch;
        pch = strtok(argv, " ");
        strcpy(args[0], pch);
        printf("args[0]: %s\n", args[0]);
        printf("pch: %s\n", pch);
        int commonNum = -1;

        while (pch != NULL)
        {
            argc++;
            printf("pch: %s\n", pch);
            commonNum = atoi(pch);
            pch = strtok(NULL, " ,.-");
            if (pch != NULL)
//                printf("pch: %s\n", pch);
                strcpy(args[argc], pch);
        }
        printf("commonNum: %d\n", commonNum);
        int actualNum = getDescriptor(commonNum);
        printf("actualNum: %d\n", actualNum);
        char* msg = "calling from other worlds\n";
        strcpy(buf, msg);
        Writen(actualNum, buf, MAXLINE);

    }
    else
    {
        printf("it's a frame.\n");
        return processFrame(argv);
    }
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
        frame* f = newFrame(atoi(args[0]), args[1], args[2], args[3]);
        enqueue(dataQ, f);
        printf("new frame added\n");
        printf("dataQ size: %d\n", dataQ->numFrames);
        return ADDED;
    } else {
        printf("argv is not a valid format: %s\n", argv);
        return INVALID;
    }
}

int reply()
{
    if (isFull(dataQ)) {
        printf("data queue full, need to send a wait signal\n");
        return REJECT;
    } else {
        printf("data not full, send signal that it can send a frame\n");
        return SEND;
    }
}


int sendToSP(int num, char* msg)
{

}


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
//#ifdef	NOTDEFF
//            printf("INET_ADDRSTRLEN: %d\n", INET_ADDRSTRLEN);
            
            //printf("new client: %s, port %d\n",
			//		Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, INET_ADDRSTRLEN),
			//		ntohs(cliaddr.sin_port));
//#endif

            for (i = 0; i < FD_SETSIZE; i++)
                if (client[i] < 0) {
                    printf("inserting into connfd (%d) into client[%d]: %d\n", connfd, i);
                    client[i] = connfd;	/* save descriptor */
                    printf("done\n");
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
