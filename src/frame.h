//
// Created by Kevin Funderburg on 12/8/20.
//

#ifndef FRAME_H
#define FRAME_H

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

FrameQueue* newQueue();

frame* top(FrameQueue* queue);

void pop();

void enqueue(FrameQueue* fq, frame* newFrame);

#endif //FRAME_H
