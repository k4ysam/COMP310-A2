#ifndef QUEUE_H
#define QUEUE_H

#include "pcb.h"

// FIFO queue of PCB pointers, implemented as a singly linked list
// PCBs are linked through their own 'next' field
typedef struct {
    PCB *head;
    PCB *tail; // maintaing tail keeps enqueue O(1)
    int  size;
} Queue;

void  queue_init(Queue *q);
void  enqueue(Queue *q, PCB *pcb); // setting new PCB's next to NULL and adds it to the tail of the queue
PCB  *dequeue(Queue *q); // removes head and updates head and tail pointers
int   queue_is_empty(Queue *q);

#endif
