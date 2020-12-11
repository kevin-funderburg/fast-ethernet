//
// Created by Kevin Funderburg on 12/8/20.
//

#ifndef FRAME_H
#define FRAME_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define MAXFRAMES 5

enum frameType {DATA, REQUEST, POS_REPLY, NEG_REPLY};

typedef struct framenode frame;

struct framenode
{
    int         seq;    //sequence number
    char*       src;    //source address
    char*       dest;   //destination address
    char*       data;   //data to send
    struct framenode* next;
};

typedef struct FrameQueue
{
    int numFrames;
    frame* head;
    frame* tail;
} FrameQueue;

frame* newFrame(int seq, char* src, char* dest, char* data);

FrameQueue* newQueue();

frame* top(FrameQueue* queue);

void pop();

void enqueue(FrameQueue* fq, frame* newFrame);

bool isFull(FrameQueue* fq);

#endif //FRAME_H
