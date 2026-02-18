#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "shellmemory.h"
#include "shell.h"
#include "pcb.h"

void scheduler_run(Queue *q) {
    while (!queue_is_empty(q)) { // runs until the queue is empty
        PCB *current = dequeue(q); // deques front PCB

        // exectures each instruction of curr processto completion ( first come first serve)
        while (current->pc < current->mem_len) {
            char *line = mem_get_frame(current->mem_start + current->pc);
            current->pc++;
            if (line != NULL)
                parseInput(line); // hands line to existing shell interpreter
        }

        // free frames and the PCB itself
        mem_free_frames(current->mem_start, current->mem_len);
        pcb_free(current);
    }
}
