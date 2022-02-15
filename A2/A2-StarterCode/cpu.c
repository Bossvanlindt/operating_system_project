#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>

#include "interpreter.h"
#include "shellmemory.h"
#include "shell.h"

//2.2.1
//Handles general CPU functionality of running either [the entire program from start
//location in shell memory to the end] or [a certain number of commands]

//cpu_run default function 
int cpu_run(int start_location, int end_location) {
    int errorCode = 0;

    for (int i = start_location; i < end_location; i++) {
        char *command = mem_get_value_by_index(i);
        errorCode = parseInput(command);
		if (errorCode == -1) exit(99);
    }
}

//cpu_run_lines runs certain number of lines
//Returns end location in mem for Round Robin to keep track in PCB
//Returns -1 if we're done executing this program
int cpu_run_lines(int start_location, int end_location, int num_lines) {
    int errorCode = 0;

    for (int i = start_location, j = 0; j < num_lines; i++, j++) {
        if (i == end_location)
            return -1;

        char *command = mem_get_value_by_index(i);
        errorCode = parseInput(command);
		if (errorCode == -1) exit(99);
    }

    return start_location + num_lines;
}