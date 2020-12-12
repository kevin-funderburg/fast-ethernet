//
// Created by Kevin Funderburg on 12/8/20.
//

#ifndef FRAME_H
#define FRAME_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
//#include "tcp.h"

#define MAXFRAMES 5
#define none 0
#define CSP_ADDR 9999

#define SEND_MSG "send"
#define WAIT_MSG "wait"
#define REQ_MSG "request"

enum frameType {DATA = 0, REQUEST = 1, POS_REPLY = 2, NEG_REPLY = 3, FORWARD = 4};

typedef struct framenode frame;

struct framenode
{
    int                 seq;    //sequence number
    int                 src;    //source address
    int                 dest;   //destination address
    char*               data;   //data to send
    enum frameType      type;   //type of frame
    struct framenode*   next;
};

typedef struct FrameQueue
{
    int size;
    frame* head;
    frame* tail;
} FrameQueue;

frame* newFrame(int seq, int src, int dest, char* data, int type);

FrameQueue* newQueue();

frame* top(FrameQueue* queue);

void pop(FrameQueue* fq);

void enqueue(FrameQueue* fq, frame* newFrame);

bool isFull(FrameQueue* fq);

bool isEmpty(FrameQueue* fp);

frame* textToFrame(char* txt);

char* frameToText(frame* f);

char* split(char* txt, const char* seps);

void printQueue(FrameQueue* fq);

#endif //FRAME_H
