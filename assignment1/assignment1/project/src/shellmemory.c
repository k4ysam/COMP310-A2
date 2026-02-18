#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "shellmemory.h"

/* ── Frame store ─────────────────────────────────────────────────────────── */

static char *frame_store[FRAME_STORE_SIZE]; /* NULL = free slot */

static void frame_store_init() {
    int i;
    for (i = 0; i < FRAME_STORE_SIZE; i++)
        frame_store[i] = NULL;
}

int mem_load_program(FILE *p, int *start, int *length) {
    char line[FRAME_LINE_LEN + 2]; /* +2 for '\n' and '\0' */
    int count = 0;
    int i, block_start = -1, run = 0;

    /* First pass: count non-blank lines in the file. */
    while (fgets(line, sizeof(line), p) != NULL) {
        if (line[0] != '\n' && line[0] != '\0')
            count++;
    }

    if (count == 0) {
        *start = 0;
        *length = 0;
        return 0;
    }

    /* Find the first contiguous free block of 'count' frames. */
    for (i = 0; i < FRAME_STORE_SIZE; i++) {
        if (frame_store[i] == NULL) {
            if (run == 0) block_start = i;
            run++;
            if (run == count)
                break;
        } else {
            run = 0;
            block_start = -1;
        }
    }

    if (run < count)
        return 1; /* not enough contiguous space */

    /* Second pass: read lines into the allocated block. */
    rewind(p);
    int idx = block_start;
    while (fgets(line, sizeof(line), p) != NULL) {
        if (line[0] == '\n' || line[0] == '\0') continue;
        line[strcspn(line, "\r\n")] = '\0';
        frame_store[idx] = strdup(line);
        idx++;
    }

    *start  = block_start;
    *length = count;
    return 0;
}

void mem_free_frames(int start, int length) {
    int i;
    for (i = start; i < start + length; i++) {
        if (frame_store[i] != NULL) {
            free(frame_store[i]);
            frame_store[i] = NULL;
        }
    }
}

char *mem_get_frame(int idx) {
    if (idx < 0 || idx >= FRAME_STORE_SIZE) return NULL;
    return frame_store[idx];
}

/* ── Variable memory ─────────────────────────────────────────────────────── */

struct memory_struct {
    char *var;
    char *value;
};

struct memory_struct shellmemory[MEM_SIZE];

// Helper functions
int match(char *model, char *var) {
    int i, len = strlen(var), matchCount = 0;
    for (i = 0; i < len; i++) {
        if (model[i] == var[i]) matchCount++;
    }
    if (matchCount == len) {
        return 1;
    } else return 0;
}

// Shell memory functions

void mem_init(){
    int i;
    for (i = 0; i < MEM_SIZE; i++){		
        shellmemory[i].var   = "none";
        shellmemory[i].value = "none";
    }
    frame_store_init();
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {
    int i;

    for (i = 0; i < MEM_SIZE; i++){
        if (strcmp(shellmemory[i].var, var_in) == 0){
            shellmemory[i].value = strdup(value_in);
            return;
        } 
    }

    //Value does not exist, need to find a free spot.
    for (i = 0; i < MEM_SIZE; i++){
        if (strcmp(shellmemory[i].var, "none") == 0){
            shellmemory[i].var   = strdup(var_in);
            shellmemory[i].value = strdup(value_in);
            return;
        } 
    }

    return;
}

//get value based on input key
char *mem_get_value(char *var_in) {
    int i;

    for (i = 0; i < MEM_SIZE; i++){
        if (strcmp(shellmemory[i].var, var_in) == 0){
            return strdup(shellmemory[i].value);
        } 
    }
    return "Variable does not exist";
}