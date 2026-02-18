#include <stdlib.h>
#include "pcb.h"

static int pid_counter = 1;

PCB *pcb_create(int mem_start, int mem_len) {
    PCB *pcb = (PCB *)malloc(sizeof(PCB));
    if (pcb == NULL) return NULL;
    pcb->pid              = pid_counter++;
    pcb->mem_start        = mem_start;
    pcb->mem_len          = mem_len;
    pcb->pc               = 0;
    pcb->job_length_score = mem_len;
    pcb->next             = NULL;
    return pcb;
}

void pcb_free(PCB *pcb) {
    free(pcb);
}
