#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"

int parseInput(char ui[]);

// Start of everything
int main(int argc, char *argv[]) {
    printf("Shell version 1.5 created Dec 2025\n");

    char prompt = '$';  				// Shell prompt
    char userInput[MAX_USER_INPUT];		// user's input stored here
    int errorCode = 0;					// zero means no error, default

    //init user input
    for (int i = 0; i < MAX_USER_INPUT; i++) {
        userInput[i] = '\0';
    }
    
    //init shell memory
    mem_init();
    while(1) {
        // if stdin comes from terminal, we are in interactive mode, print prompt and flush to clear buffer    
        if (isatty(STDIN_FILENO)) {
            printf("%c ", prompt);
            fflush(stdout);
        }
        // if user exits/disrupts in terminal, or batch mode reaches EOF while terminal is still running, exit(0)
        if (fgets(userInput, MAX_USER_INPUT-1, stdin) == NULL) break;
        
        errorCode = parseInput(userInput);
        if (errorCode == -1) exit(99);	// ignore all other errors
        memset(userInput, 0, sizeof(userInput));
    }

    return 0;
}

int wordEnding(char c) {
    // was easier to handle semicolon separately for command chaining
    return c == '\0' || c == '\n' || c == ' ';
}

// Changed original parseInput into parseSingle helper function to parse and execute a single command
int parseSingle(char *inp) {
    char tmp[200], *words[100];                            
    int ix = 0, w = 0;
    int wordlen;
    int errorCode;
    
    for (ix = 0; inp[ix] == ' ' && ix < 1000; ix++); // skip white spaces
    while (inp[ix] != '\n' && inp[ix] != '\0' && ix < 1000) {
        // extract a word
        for (wordlen = 0; !wordEnding(inp[ix]) && ix < 1000; ix++, wordlen++) {
            tmp[wordlen] = inp[ix];                        
        }
        tmp[wordlen] = '\0';
        words[w] = strdup(tmp);
        w++;
        if (inp[ix] == '\0') break;
        ix++; 
    }
    errorCode = interpreter(words, w);
    
    // free allocated memory for words
    for (int i = 0; i < w; i++) {
        free(words[i]);
    }
    
    return errorCode;
}

int parseInput(char inp[]) {
    char commands[10][MAX_USER_INPUT];  // support for upto 10 chained commands
    int cmdCount = 0;
    int ix = 0, cmdLen = 0;
    int errorCode = 0;
    
    // splitting input by semicolons into separate commands
    for (ix = 0; inp[ix] != '\n' && inp[ix] != '\0' && ix < 1000; ix++) {
        if (inp[ix] == ';') {
            commands[cmdCount][cmdLen] = '\0';
            cmdCount++;
            cmdLen = 0;
            if (cmdCount >= 10) break;  // max 10 commands
        } else {
            commands[cmdCount][cmdLen] = inp[ix];
            cmdLen++;
        }
    }
    // adding last command (or the only command if no semicolons)
    commands[cmdCount][cmdLen] = '\0';
    cmdCount++;
    
    // executing each command in the original sequence
    for (int i = 0; i < cmdCount; i++) {
        errorCode = parseSingle(commands[i]);
        if (errorCode == -1) return errorCode;  // quit command
    }
    
    return errorCode;
}
