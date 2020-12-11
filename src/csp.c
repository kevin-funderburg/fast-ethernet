#include "tcp.h"

FrameQueue* dataQ;
FrameQueue* reqQ;

bool        queuesSet = false;

void getInstr(char* line)
{

}

void argparse(char* argv)
{
    // parse args
    char args[10][25];
    char *pch;
    pch = strtok(argv, ",");
    strcpy(args[0], pch);
    int argc = 0;
    while (pch != NULL)
    {
        argc++;
        pch = strtok(NULL, " ,.-\n");
        if (pch != NULL)
            strcpy(args[argc], pch);
    }
    printf("parsed args:\n");
    for (int i = 0; i < argc; ++i)
        printf("args[%d]\t%s\n", i, args[i]);

    frame* newFrame = (frame *) malloc(sizeof(frame));
    newFrame->seq = atoi(args[0]);
    newFrame->src = args[1];
    newFrame->dest = args[2];
    newFrame->data = args[03];
    enqueue(dataQ, newFrame);
    printf("dataQ size: %d\n", dataQ->numFrames);
}

/**
 * we will assume for now that the station will send the csp 'frame' that looks like:
 * "sequence,srcaddress,destinationaddr,data"
 */
int main(int argc, char **argv)
{
    int					i, maxi, maxfd, listenfd, connfd, sockfd;
    int					nready, client[FD_SETSIZE];
    ssize_t				n;
    fd_set				rset, allset;       //set of file descriptors
    char				buf[MAXLINE];
    socklen_t			clilen;
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
        nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &rset)) {	/* new client connection */
            clilen = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
            //printf("attempting to get \n", );
            printf("new client added: %s, port %d\n", "fix later", ntohs(cliaddr.sin_port));
//#ifdef	NOTDEFF
            printf("INET_ADDRSTRLEN: %d\n", INET_ADDRSTRLEN);  
            
            //printf("new client: %s, port %d\n",
			//		Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, INET_ADDRSTRLEN),
			//		ntohs(cliaddr.sin_port));
//#endif

            for (i = 0; i < FD_SETSIZE; i++)
                if (client[i] < 0) {
                    client[i] = connfd;	/* save descriptor */
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




        for (i = 0; i <= maxi; i++) {	/* check all clients for data */
            if ( (sockfd = client[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &rset)) {
                if ( (n = read(sockfd, buf, MAXLINE)) == 0) {
                    /*4connection closed by client */
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                } else
                {
                    argparse(buf);
                    Writen(sockfd, buf, n);
                }

                if (--nready <= 0)
                    break;				/* no more readable descriptors */
            }
        }
    }
}
