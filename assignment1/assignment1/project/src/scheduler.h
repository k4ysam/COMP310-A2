#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "queue.h"

// Runing all processes in the ready queue to completion using FCFS
// Each process executes its instructions one at a time in order
// When a process finishes, its frame-store lines are freed and the PCB is destroyed
void scheduler_run(Queue *q);

#endif
