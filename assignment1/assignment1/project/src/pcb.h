#ifndef PCB_H
#define PCB_H

struct PCB {
    int pid;              //unique pid
    int mem_start;        //first frame index in the frame store
    int mem_len;          //number of frames (lines) belonging to this process
    int pc;               //program counter: starts at 0 and increments for each instruction
    int job_length_score; //initialised to mem_len, used by SJF / AGING
    struct PCB *next;     //intrusive pointer so PCB can link itself to the ready queue without extra allocations
};

typedef struct PCB PCB;

PCB *pcb_create(int mem_start, int mem_len);  // mallocs a PCB, fills in the fields and returns a pointer to the PCB
void pcb_free(PCB *pcb);

#endif
