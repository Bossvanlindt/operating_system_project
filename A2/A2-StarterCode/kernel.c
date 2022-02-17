//2.2.1
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>

#include "interpreter.h"
#include "shellmemory.h"
#include "cpu.h"
#include "shell.h"
#include "kernel.h"


//2.2.1 PCB struct
struct PCB {
	int PID;
	int start_location;
//	int firstLine; YOUNES: I'm not sure why we need this? Maybe it's not needed when we do this by index instead
// by firstLine i just meant the index of the first line, but start_location is a better name
	int end_location;
	int current_location;//Added this because we want to know where the process is when going back to it after switching
};

//2.2.1 ready queue
struct ReadyQueue {
	struct PCB *current;
	struct PCB *next;
};


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
int kernel(char *file1, char *file2, char *file3, char *policy);
int save_to_memory(char *file,int *locations);
int create_pcb(char *file);
void add_pcb_to_queue(struct PCB *process);    //Given that queue would maximally be 3 long, couldn't we simply make an array of PCBs?
//2.2.1 error if memory full
int notEnoughMemory();
int sameFileNames();

//kernel method handles all kernel logic
//Returns 0 if all went well
//Returns -1 if something went wrong
/*int kernel(char *file1, char *file2, char *file3, char *policy) {
    int errorCode = 0;
	
	//Save & create pcb for each file
    //If file1 empty, something's wrong cuz no files at all
    if (!file1)
        return badcommand();
	}	

	if (strcmp(file1,file2) == 0 || strcmp(file1,file3) == 0 || strcmp(file2,file3) == 0) {
		return sameFileName();
	}

	errorCode = create_pcb(file1);

	if (errorCode) {
		return notEnoughMemory();
	}

	if (file2) {
		errorCode = create_pcb(file2);
		if (errorCode) {
			//Add Cleanup method
			return notEnoughMemory();
		}
	}

	if (file3) {
		errorCode = create_pcb(file3);
		if (errorCode) {
			//Add cleanup method
			return notEnoughMemory();
		}
	}

   return errorCode;
}*/

int create_pcb(char *file) {
	int errorCode = 0;
	struct PCB *process = (struct PCB*) malloc(sizeof(struct PCB));
	int first_last[2] = {-1,-1};
	
	errorCode = save_to_memory(file, first_last);
	//2.2.1First and last location were saved in the save_to_memory function
	process->start_location = first_last[0];
	process->end_location = first_last[0];
	process->current_location = 0; 
	
	add_pcb_to_queue(process);

	return errorCode;
	 
}

//Saves the file's contents into shell memory
//Returns start_location and end_location to create the PCB
int save_to_memory(char *file, int *locations) {


    int errCode = 0;
	char line[1000];
	FILE *p = fopen(file,"rt");  // the program is in a file
	int position = -1;

	if(p == NULL){
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	
	int first = mem_set_line(line);

	if (first == -1) notEnoughMemory(); 
	locations[0] = first;
	
	while(1){

		fgets(line,999,p);
		position = mem_set_line(line);
		if (position == -1) notEnoughMemory(); 

		if(feof(p)){
			//2.2.1 Records last index of script line
			locations[1] = position;
			break;
		}
	}

    fclose(p);
	return 0;
}



int notEnoughMemory() {
	printf("%s\n", "Not enough memory"); 
	return 5;
}

int sameFileName() {
	printf("%s\n", "Same file inputted twice"); 
	return 6;
}