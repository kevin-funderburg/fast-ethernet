//
// Created by Kevin Funderburg on 12/8/20.
//

#ifndef FRAME_H
#define FRAME_H

typedef struct framenode frame;

struct framenode
{
    int    seq;    //sequence number
    int    src;    //source address
    int    dest;   //destination address
    int    data;   //data to send
    struct frame* next;
};

typedef struct FrameQueue
{
    frame* head, tail;
} FrameQueue;

FrameQueue* frameQueue();

frame* top();

void pop();

void push(frame* newFrame);

#endif //FRAME_H
