//
// Created by Kevin Funderburg on 12/8/20.
//

#ifndef FRAMEQ_H
#define FRAMEQ_H

struct frame
{
    int    seq;    //sequence number
    int    src;    //source address
    int    dest;   //destination address
    int    data;   //data to send
    frame* next;
};

typedef struct FrameQueue
{
    frame* head,
           tail;
} FrameQueue;

FrameQueue* frameQueue();

frame* top();

void pop();

void push(frame* newFrame);

#endif //FRAMEQ_H
