#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shellmemory.h"
#include "shell.h"
#include "pcb.h"
#include "queue.h"
#include "scheduler.h"


int MAX_ARGS_SIZE = 10; // changed to 10 to support run command

int badcommand() {
    printf("Unknown Command\n");
    return 1;
}

// For source command only
int badcommandFileDoesNotExist() {
    printf("Bad command: File not found\n");
    return 3;
}

int help();
int quit();
int set(char *var, char *value);
int echo(char *str);
int my_ls();
int my_mkdir(char *dirname);
int my_touch(char *filename);
int my_cd(char *dirname);
int print(char *var);
int run(char *command_args[], int args_size);
int source(char *script);
int badcommandFileDoesNotExist();

// Interpret commands and their arguments
int interpreter(char *command_args[], int args_size) {
    int i;

    if (args_size < 1 || args_size > MAX_ARGS_SIZE) {
        return badcommand();
    }

    for (i = 0; i < args_size; i++) {   // terminate args at newlines
        command_args[i][strcspn(command_args[i], "\r\n")] = 0;
    }

    if (strcmp(command_args[0], "help") == 0) {
        //help
        if (args_size != 1)
            return badcommand();
        return help();

    } else if (strcmp(command_args[0], "quit") == 0) {
        //quit
        if (args_size != 1)
            return badcommand();
        return quit();

    } else if (strcmp(command_args[0], "set") == 0) {
        //set
        if (args_size != 3)
            return badcommand();
        return set(command_args[1], command_args[2]);

    } else if (strcmp(command_args[0], "echo") == 0) {
        // echo
        if (args_size != 2)
            return badcommand();
        return echo(command_args[1]);

    } else if (strcmp(command_args[0], "my_ls") == 0) {
        // my_ls
        if (args_size != 1)
            return badcommand();
        return my_ls();
        
    } else if (strcmp(command_args[0], "my_mkdir") == 0) {
        // my_mkdir
        if (args_size != 2)
            return badcommand();
        return my_mkdir(command_args[1]);
    
    } else if (strcmp(command_args[0], "my_touch") == 0) {
        // my_touch
        if (args_size != 2)
            return badcommand();
        return my_touch(command_args[1]);
    
    } else if (strcmp(command_args[0], "my_cd") == 0) {
        // my_cd
        if (args_size != 2)
            return badcommand();
        return my_cd(command_args[1]);

    } else if (strcmp(command_args[0], "print") == 0) {
        if (args_size != 2)
            return badcommand();
        return print(command_args[1]);

    } else if (strcmp(command_args[0], "run") == 0) {
        // run
        if (args_size < 2)
            return badcommand();
        return run(command_args, args_size);

    } else if (strcmp(command_args[0], "source") == 0) {
        if (args_size != 2)
            return badcommand();
        return source(command_args[1]);
    } else {
        return badcommand();
    }
}

int help() {
    
    // note the literal tab characters here for alignment
    char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
source SCRIPT.TXT	Executes the file SCRIPT.TXT\n ";
    printf("%s\n", help_string);
    return 0;
}

int quit() {
    printf("Bye!\n");
    exit(0);
}

int set(char *var, char *value) {
    // Challenge: allow setting VAR to the rest of the input line,
    // possibly including spaces.

    // Hint: Since "value" might contain multiple tokens, you'll need to loop
    // through them, concatenate each token to the buffer, and handle spacing
    // appropriately. Investigate how `strcat` works and how you can use it
    // effectively here.

    mem_set_value(var, value);
    return 0;
}


int print(char *var) {
    printf("%s\n", mem_get_value(var));
    return 0;
}

int source(char *script) { 
    FILE *p = fopen(script, "rt");
    if (p == NULL)
        return badcommandFileDoesNotExist();

    int start = 0, length = 0;
    int err = mem_load_program(p, &start, &length); // changed old source to load all program into frame store
    fclose(p);

    if (err != 0) {
        printf("Bad command: Not enough memory\n");
        return err;
    }

    PCB *pcb = pcb_create(start, length); // create a PCB for the program
    if (pcb == NULL) {
        mem_free_frames(start, length);
        printf("Bad command: Could not create process\n");
        return 1;
    }

    Queue q; // init local queue
    queue_init(&q);
    enqueue(&q, pcb);
    scheduler_run(&q);

    return 0;
}

// helper
int is_alphanumeric(char c) {
    return ((c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_');
}

// helper
int is_var_alpha_str(char *str) {
    if (!(*str == '$' || is_alphanumeric(*str))) return 0;
    
    str++;
    while (*str) {
        if (!is_alphanumeric(*str)) return 0;
        str++;
    }

    return 1;
}

/*
Takes one [$]alphanumeric_string as input and prints it.
If the string is a standard alphanumeric string, echo displays it on a new line.
If the string precedes a $, echo displays the value stored by the variable $alphanumeric_string, "" by default, on a new line.
The function returns the command prompt to ther user and raises an error if the input is not [$]alphanumeric_string.
*/
int echo(char *str) {
    char *start = str;

    // error checking, if not [$]alphanumeric_string return bad command;
    if (!is_var_alpha_str(str)) return badcommand();

    if (*start == '$' && *(start + 1)) { // echo variable value, "" if nothing is stored
        start = mem_get_value((start + 1)); // print $(VAR_NAME)
        if (!strcmp(start, "Variable does not exist")) start = "";
    } // else print regular string
    printf("%s\n", start);

    return 0;
}

/*
Takes nothing as input and lists all files present in the current directory.
If the current directory contains other directories, my_ls lists only the names of said directories.
Each file or directory name is displayed on a new line.
File/directory names are listed in alphabetical order.
    - Names starting with a number follow natural ordering and appear before lines starting with a letter.
    - Names starting with an uppercase letter appear before those starting with the same letter in lower case.
Includes . and .. (if there exists a parent) directories displayed at the top of the listing.
Returns the comand prompt to the user.
*/
int my_ls() {
    // list entries in . and sort them by name alphanumerically according to the rules above
    struct dirent **entries;
    int n = scandir(".", &entries, NULL, alphasort); // compiles but flagged in VSCode
    for (int i = 0; i < n; i++) {
        printf("%s\n", entries[i]->d_name);
        free(entries[i]);
    }
    free(entries);
    
    return 0;
}

// helper
int badcommand_mkdir() {
    printf("Bad command: my_mkdir\n");
    return 1;
}

// helper
int badcommand_cd() {
    printf("Bad command: my_cd\n");
    return 1;
}

/*
Takes as input an [$]alpha_numeric string dirname and creates a new directory with the name dirname in '.'.
If dirname is preceded by $ then:
- If $dirname exists in the memory and contains a single alpha_numeric token (default behaviour) then we create the directory with this name.
- Otherwise displays returns badcommand().
In any case, returns command prompt to the user.
*/
int my_mkdir(char *dirname) {
    char *start = dirname;

    // error checking, if not [$]alphanumeric_string return bad command;
    if (!is_var_alpha_str(dirname)) return badcommand_mkdir();

    if (*start == '$' && *(start + 1)) { // mkdir variable value, return badcommand() if nothing is stored
        start = mem_get_value((start + 1)); // get value from memory
        if (!strcmp(start, "Variable does not exist")) return badcommand_mkdir();
        // check if the value is a single alphanumeric token
        if (!is_var_alpha_str(start)) return badcommand_mkdir();
    }

    // create directory, return error if cannot create
    mkdir(start, 0755);
    
    return 0;
}

/*
Takes an alpha_numeric string filename as input and creates a new empty file inside the current directory.
Returns command prompt to the user.
*/
int my_touch(char *filename) {
    if(*filename == '$' || !is_var_alpha_str(filename)) return badcommand();

    FILE *file = fopen(filename, "w");
    if (file == NULL) return badcommand(); // if file could not be created, return error
    fclose(file);

    return 0;
}

/*
Takes as input an alpha_numeric string dirname within the current directory and changes to it.
If the directory is not alpha_numeric, or we cannot change into it (say if there does not exist such a directory accessible from our current working path), then we return error.
Returns the command prompt to the user.
*/
int my_cd(char *dirname) {
    if(*dirname == '$' || !is_var_alpha_str(dirname)) return badcommand_cd();

    if (chdir(dirname) == -1) return badcommand_cd(); // if cannot cd return error

    return 0;
}

/*
Implements the fork-exec-wait pattern to run external commands.
Takes the command and its arguments, forks a child process, executes the command in the child, and waits for it to complete in the parent.
*/
int run(char *command_args[], int args_size) {
    pid_t pid;
    int status;
    
    // choosing execvp over execl/execlp as it uses PATH automatically to find the command

    // create argument array for execvp (one less for "run", one more for NULL)
    char *exec_args[args_size];
    for (int i = 1; i < args_size; i++) {
        exec_args[i - 1] = command_args[i]; // skip "run" command
    }
    exec_args[args_size - 1] = NULL; // NULL-terminate the array
    

    // output order was wrong due to buffering therefore flushing stdout before forking to avoid duplicate buffered output
    fflush(stdout);
    
    // fork-exec-wait pattern
    pid = fork();
    
    if (pid < 0) {
        // fork failed
        return badcommand();
    } else if (pid == 0) {
        // child process, exec the command
        execvp(exec_args[0], exec_args);
        // if exec returns, it failed
        exit(1);
    } else {
        // parent process, wait for child to complete
        wait(&status);
    }
    
    return 0;
}
