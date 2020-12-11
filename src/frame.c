//
// Created by Kevin Funderburg on 12/8/20.
//

#include "frame.h"

frame* newFrame(int seq, char* src, char* dest, char* data)
{
    frame *newFrame = (frame *) malloc(sizeof(frame));
    newFrame->seq = seq;
    newFrame->src = src;
    newFrame->dest = dest;
    newFrame->data = data;
    return newFrame;
}


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
    else {
        fq->tail->next = newFrame;
        fq->tail = fq->tail->next;
    }
    fq->numFrames++;
}

bool isFull(FrameQueue* fq) {return fq->numFrames == MAXFRAMES;}
