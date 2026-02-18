#include <stdlib.h>
#include "queue.h"

void queue_init(Queue *q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

// setting new PCB's next to NULL and adds it to the tail of the queue
void enqueue(Queue *q, PCB *pcb) {
    pcb->next = NULL;
    if (q->tail == NULL) {
        q->head = pcb;
        q->tail = pcb;
    } else {
        q->tail->next = pcb;
        q->tail       = pcb;
    }
    q->size++;
}

// removing head and updates head and tail pointers
PCB *dequeue(Queue *q) {
    if (q->head == NULL) return NULL;
    PCB *pcb  = q->head;
    q->head   = q->head->next;
    if (q->head == NULL) q->tail = NULL;
    pcb->next = NULL;
    q->size--;
    return pcb;
}

int queue_is_empty(Queue *q) {
    return q->head == NULL;
}
