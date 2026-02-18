#ifndef PCB_H
#define PCB_H

struct PCB {
    int pid;              /* unique process id */
    int mem_start;        /* first frame index in the frame store */
    int mem_len;          /* number of frames (lines) belonging to this process */
    int pc;               /* program counter: offset from mem_start, 0-based */
    int job_length_score; /* used by SJF / AGING policies */
    struct PCB *next;     /* intrusive link for the ready queue */
};

typedef struct PCB PCB;

PCB *pcb_create(int mem_start, int mem_len);
void pcb_free(PCB *pcb);

#endif
