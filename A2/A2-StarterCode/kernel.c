//2.2.1
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>

#include "interpreter.h"
#include "shellmemory.h"
#include "cpu.h"
#include "shell.h"

//Kernel handles all aspects of scheduling and managing the exec function of the shell
//Rundown: 
//  1. Kernel function gets called with up to three programs and the desired scheduling policy
//  2. For each program, this function calls:
//      a. save_to_memory to save it to the shell memory
//      b. create_pcb
//      c. add_to_queue to add new pcb to the ready queue
//  3. Kernel function lastly calls the desired scheduler function according to the selected
//     policy to execute it all. 
// This way, the default of the run command in the shell is kernel(prog, NULL, NULL, "FCFS")

int save_to_memory(char *file);
struct PCB* create_pcb(int start_location, int end_location);
void add_pcb_to_queue();    //Given that queue would maximally be 3 long, couldn't we simply make an array of PCBs?

//kernel method handles all kernel logic
//Returns 0 if all went well
//Returns -1 if something went wrong
int kernel(char *file1, char *file2, char *file3, char *policy) {
    //Save & create pcb for each file
    //If file1 empty, something's wrong cuz no files at all
    if (file1 == NULL)
        return -1;
    
}

//Saves the file's contents into shell memory
//Returns start_location and end_location to create the PCB
int save_to_memory(char *file) {


    //TO YOUNES: I kept your code in case we need it
    // int errCode = 0;
	// char line[1000];
	// FILE *p = fopen(script,"rt");  // the program is in a file
	// int lastPos;

	// if(p == NULL){
	// 	return badcommandFileDoesNotExist();
	// }

	// fgets(line,999,p);
	// struct PCB *process = (struct PCB*) malloc(sizeof(struct PCB));
	// int first = mem_set_line(line);

	// if (first == -1) notEnoughMemory();
	// //2.2.1 Records first line of script line in shell memory
	// process->firstLine = first;
	// while(1){
		
		
	// 	/*//1.2.5 if line is a one-liner with multiple chained commands, do like in main in shell.c
	// 	if (strchr(line, ';') != NULL) {
	// 		char *command = strtok(line, ";");
	// 		while (command != NULL) {
	// 			errCode = parseInput(command);
	// 			if (errCode == -1) exit(99);
	// 			command = strtok(NULL, ";");
	// 		}
	// 	} else {
	// 		errCode = parseInput(line);	// which calls interpreter()
	// 		memset(line, 0, sizeof(line));
	// 	}*/

	// 	fgets(line,999,p);
	// 	lastPos = mem_set_line(line);
	// 	if (lastPos == -1) notEnoughMemory();

	// 	if(feof(p)){
	// 		//2.2.1 Records last index of script line
	// 		process->lastLine = lastPos;
	// 		break;
	// 	}
	// }

    // fclose(p);

	// addReadyQueue(process);
	// return 0;
}