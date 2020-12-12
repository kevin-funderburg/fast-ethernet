//
// Created by Kevin Funderburg on 12/8/20.
//

#include "frame.h"
#define MAXLINE 4096

frame* newFrame(int seq, int src, int dest, char* data, int type)
{
    printf("newFrame:\n");
    printf("seq: %d\n", seq);
    printf("src: %d\n", src);
    printf("dest: %d\n", dest);
    printf("data: %s\n", data);
    printf("newFrame: %d,%d,%d,%s,%d\n", seq,src,dest,data,type);
    frame *newFrame = (frame *) malloc(sizeof(frame));
    newFrame->seq = seq;
    newFrame->src = src;
    newFrame->dest = dest;
    newFrame->data = data;
    newFrame->type = type;
    newFrame->next = NULL;
    return newFrame;
}


FrameQueue* newQueue()
{
    FrameQueue* this = (FrameQueue *) malloc(sizeof(struct FrameQueue));
    this->size = 0;
    this->head = NULL;
    this->tail = NULL;
    return this;
}


frame* top(FrameQueue* fq)
{
    printf("...top()...\n");
    return fq->head;
    printf("...done | top()...\n");
}


void pop(FrameQueue* fq)
{
    if (!isEmpty(fq)) {
        frame *tempPtr = fq->head;
        fq->head = fq->head->next;
        free(tempPtr);
        fq->size--;
    }
}


void enqueue(FrameQueue* fq, frame *newFrame)
{
    printf("...enqueue()...\n");
//    printf("newFrame: %d,%d,%d,%s,%d\n", newFrame->seq, newFrame->src, newFrame->dest, newFrame->data, newFrame->type);
    if (isEmpty(fq)) { //empty list
        printf("\tqueue is empty\n");
        fq->head = newFrame;
    } else if (fq->head->seq > newFrame->seq)   //add to front
    {
        printf("\tadding to head\n");
        newFrame->next = fq->head;
        fq->head = newFrame;
    }
    else
    {
        frame *cursor = fq->head;
        while (cursor != NULL)
        {
            if ((cursor->seq < newFrame->seq) && (cursor->next == NULL))  //add to tail
            {
                printf("\tadding to tail\n");
                cursor->next = newFrame;
                break;
            }
            else if ((cursor->seq < newFrame->seq) && (cursor->next->seq > newFrame->seq))   //add inside
            {
                printf("\tadding inside\n");
                newFrame->next = cursor->next;
                cursor->next = newFrame;
                break;
            }
            else
                cursor = cursor->next;
        }
    }
    fq->size++;
    printf("\tfq->size: %d\n", fq->size);
//    printf("\t\tfq->head->type: %d\n", fq->head->type);
//    printf("\t\tfq->head->seq: %d\n", fq->head->seq);
//    printf("\t\tfq->head->src: %d\n", fq->head->src);
//    printf("\t\tfq->head->dest: %d\n", fq->head->dest);
//    printQueue(fq);
    printf("\n...done | enqueue()...\n");
}


bool isFull(FrameQueue* fq) { return fq->size == MAXFRAMES; }


bool isEmpty(FrameQueue* fp) { return fp->head == NULL; }


char* frameToText(frame* f)
{
    printf("\n...frameToText()...\n");
    char frameTxt[MAXLINE];

    printf("f->type: %d\n", f->type);
    switch (f->type) {
        case REQUEST:
            printf("CASE REQUEST\n");
            snprintf(frameTxt, MAXLINE, "request\n");
//            printf("frameTxt: %s\n", frameTxt);
            strcat(frameTxt,"request");
            break;
        case NEG_REPLY:
            strcat(frameTxt,"wait");
            break;
        case POS_REPLY:
            strcat(frameTxt,"send");
            break;
        case DATA:
            printf("CASE DATA\n");
            printf("f->seq: %d\n", f->seq);
            snprintf(frameTxt, MAXLINE, "%d,%d,%d,%s,%d\n", f->seq, f->src, f->dest, f->data, f->type);
            break;
        default:
            printf("invalid frame type\n");
            break;
    }
    printf("...done...\n");
    return frameTxt;
}


frame* textToFrame(char* txt)
{
    printf("...textToFrame()...\n");
    frame *f;

    if ((strncmp(" To ", txt, 4) == 0))
//    if ((strncmp("request", txt, 7) == 0))
    {
        char strArr[strlen(txt)];
        strcpy(strArr, txt);
        char args[10][25];
        char *pch;
        int argc = 0;
        printf("txt: %s\n", txt);
        pch = strtok(strArr, ",");
        strcpy(args[0], pch);
        char* data;
        while (pch != NULL)
        {
            argc++;
            pch = strtok(NULL, ",");
            if (pch != NULL) {
                printf("pch: %s\n", pch);
                if (argc == 2)
                    data = pch;
                else
                    strcpy(args[argc], pch);
            } else {
                printf("pch == NULL\n");
                break;
            }
        }
        printf("break\n");

        for (int i = 0; i < argc; ++i)
            printf("args[%d]: %s\n", i, args[i]);


        f = newFrame(none, none, atoi(args[1]), "request", REQUEST);
    }
    else if ((strncmp("wait", txt, 4) == 0))
    {
        f = newFrame(none, none, none, "wait", NEG_REPLY);
    }
    else if ((strncmp("send", txt, 4) == 0))
    {
        f = newFrame(none, none, none, "send", POS_REPLY);
    }
    else
    {
        if (strlen(txt) > 5)
        {
            char strArr[strlen(txt)];
            strcpy(strArr, txt);
            char args[10][25];
            char *pch;
            int argc = 0;
            printf("txt: %s\n", txt);
            pch = strtok(strArr, ",");
            strcpy(args[0], pch);
            char *data;
            while (pch != NULL) {
                argc++;
                pch = strtok(NULL, ",");
                if (pch != NULL) {
                    printf("pch: %s\n", pch);
                    if (argc == 3)
                        data = pch;
                    else
                        strcpy(args[argc], pch);
                } else {
                    printf("pch == NULL\n");
                    break;
                }
            }
            printf("break\n");

            for (int i = 0; i < argc; ++i)
                printf("args[%d]: %s\n", i, args[i]);

            f = newFrame(atoi(args[0]), atoi(args[1]), atoi(args[2]), data, DATA);
        } else {
            printf("txt is too short to make a frame: %s\n", txt);
            return NULL;
        }
    }

    return f;
}


char* split(char* txt, const char* seps)
{
    printf("...split()...\n");
    char strArr[strlen(txt)];
    strcpy(strArr, txt);
    char args[10][25];

    char *pch;
//    char dyn[];
    int argc = 0;
//    char* buf[MAXLINE];
    printf("txt: %s\n", txt);
    printf("seps: %s\n", seps);
    pch = strtok(strArr, seps);
    strcpy(args[0], pch);
    while (pch != NULL)
    {
        argc++;
        pch = strtok(NULL, seps);
        if (pch != NULL) {
            printf("pch: %s\n", pch);
            strcpy(args[argc], pch);
        } else {
            printf("pch == NULL\n");
            break;
        }
    }
    printf("break\n");
//    char* items[argc];
    char* items = malloc(argc * sizeof(char));

//    char* item = args[0];
//    printf("item: %s\n", item);
//    bzero(items, sizeof(items));
//    dyn = (char *) malloc(sizeof())
    for (int i = 0; i < argc; ++i) {
        printf("args[%d]: %s\n", i, args[i]);
        strcpy(items[i], &args[i]);
//        items[i] = args[i];
        printf("items[i]: %s\n", items[i]);
    }
    return items;
}

void printQueue(FrameQueue* fq)
{
    frame* cursor = fq->head;
    while (cursor != NULL)
    {
        printf("cursor->seq: %d\n", cursor->seq);
        cursor = cursor->next;
    }
}