#define MEM_SIZE 1000

/* Frame store for program code lines */
#define FRAME_STORE_SIZE 1000
#define FRAME_LINE_LEN   101

void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);

/* Load all lines of an open file into the frame store.
   Sets *start to the first allocated frame index and *length to the
   number of lines loaded.  Returns 0 on success, non-zero on error. */
int  mem_load_program(FILE *p, int *start, int *length);

/* Release the frames in [start, start+length) back to the free pool. */
void mem_free_frames(int start, int length);

/* Return the line string stored at frame index idx. */
char *mem_get_frame(int idx);
