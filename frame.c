//
// Created by Kevin Funderburg on 12/8/20.
//
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "frame.h"

FrameQueue* frameQueue()
{
    FrameQueue* this = (FrameQueue *) malloc(sizeof(FrameQueue));
    this->head = NULL;
    this->tail = NULL;
}


frame* top()
{
    return fq_head;
}


void pop()
{
    frame *tempPtr = fq_head;
    fq_head = fq_head->fq_next;
    delete tempPtr;
}


void push(frame *newFrame)
{
    if (fq_head == nullptr)  //empty list
        fq_head = newFrame;
    else if (fq_head->time > newFrame->time)   //add to front
    {
        newFrame->fq_next = fq_head;
        fq_head = newFrame;
    }
    else
    {
        event *fq_cursor = fq_head;
        while (fq_cursor != nullptr)
        {
            if ((fq_cursor->time < newFrame->time) && (fq_cursor->fq_next == nullptr))  //add to tail
            {
                fq_cursor->fq_next = newFrame;
                break;
            }
            else if ((fq_cursor->time < newFrame->time) && (fq_cursor->fq_next->time > newFrame->time))   //add inside
            {
                newFrame->fq_next = fq_cursor->fq_next;
                fq_cursor->fq_next = newFrame;
                break;
            }
            else
                fq_cursor = fq_cursor->fq_next;
        }
    }
}
