//
// Created by Kevin Funderburg on 12/8/20.
//
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "frame.h"

FrameQueue* newQueue()
{
    FrameQueue* this = (FrameQueue *) malloc(sizeof(struct FrameQueue));
    this->numFrames = 0;
    return this;
}


frame* top(FrameQueue* fq)
{
    return fq->head;
}


void pop(FrameQueue* fq)
{
    frame *tempPtr = fq->head;
    fq->head = fq->head->next;
    free(tempPtr);
    fq->numFrames--;
}


//void enqueue(FrameQueue* fq, frame *newFrame)
//{
//    if (fq->head == NULL)  //empty list
//        fq->head = newFrame;
//    else if (fq->head->seq > newFrame->seq)   //add to front
//    {
//        newFrame->next = fq->head;
//        fq->head = newFrame;
//    }
//    else
//    {
//        frame *fq_cursor = fq->head;
//        while (fq_cursor != NULL)
//        {
//            if ((fq_cursor->seq < newFrame->seq) && (fq_cursor->next == NULL))  //add to tail
//            {
//                fq_cursor->next = newFrame;
//                break;
//            }
//            else if ((fq_cursor->seq < newFrame->seq) && (fq_cursor->next->seq > newFrame->seq))   //add inside
//            {
//                newFrame->next = fq_cursor->next;
//                fq_cursor->next = newFrame;
//                break;
//            }
//            else
//                fq_cursor = fq_cursor->next;
//        }
//    }
//}


void enqueue(FrameQueue* fq, frame *newFrame)
{
    if (fq->tail == NULL)  //empty list
        fq->head = fq->tail = newFrame;
    else
    {
//        frame* tmp;
//        tmp = fq->tail;
        fq->tail->next = newFrame;
        fq->tail = fq->tail->next;
//        free(tmp);
    }
    fq->numFrames++;
}
