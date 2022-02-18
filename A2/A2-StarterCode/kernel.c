//2.2.1
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>

#include "interpreter.h"
#include "shellmemory.h"
#include "cpu.h"
#include "shell.h"
#include "kernel.h"



//PCB AND READY QUEUE
//PCB struct
struct PCB {
	int PID;
	int start_location;
	int end_location;
	//To know where the process is when going back to it after switching
	int current_location;
	//Queue stuff
	struct PCB *next;
};
//Ready queue
struct ReadyQueue {
	struct PCB *head;
	struct PCB *tail;
};
//Only queue actions are pop & add
void add_to_queue(struct PCB *pcb, struct ReadyQueue *queue) {
	if (!queue->head)
		queue->head = pcb;
	queue->tail->next = pcb;
	queue->tail = pcb;
}
struct PCB* pop_off_queue(struct ReadyQueue *queue) {
	//If last to pop, set head to NULL
	struct PCB *res = queue->head;
	queue->head = res->next;
	return res;
}



int kernel(char *file1, char *file2, char *file3, char *policy);
int* save_to_memory(char *file, int *first_last);
int process_file(char *file, struct ReadyQueue *queue);
void add_to_queue(struct PCB *pcb, struct ReadyQueue *queue);
void clear_prog(int start_location, int end_location);
int notEnoughMemory();
int sameFileNames();
//List of policies
int FCFS(struct ReadyQueue *queue);
// int SJF(struct ReadyQueue *queue);
// int RR(struct ReadyQueue *queue);
// int AGING(struct ReadyQueue *queue);



//kernel method handles all kernel logic
//Returns 0 if all went well
//Returns -1 if something went wrong
int kernel(char *file1, char *file2, char *file3, char *policy) {
    int errorCode = 0;
	
	//Input checking of files
    //If file1 empty, no files at all so bad command
    if (!file1)
        return badcommand();
	//If any of the files are identical, wrong inputs as need to have unique for each. This works because 
	//we always fill in file1 before file2 and file2 before file3
	if (file1 && file2) {
		if (strcmp(file1, file2) == 0)
			return sameFileNames();
	}
	if (file3) {
		if (strcmp(file1, file3) == 0 || strcmp(file2, file3) == 0)
			return sameFileNames();
	}

	//ReadyQueue instantiation
	struct ReadyQueue *queue = malloc(sizeof(struct ReadyQueue));
	queue->head = NULL;
	queue->tail = NULL;

	//Add all programs to shell memory, create their PCBs, and add them to the ready queue
	process_file(file1, queue);
	if (file2)
		process_file(file2, queue);
	if (file3)
		process_file(file3, queue);

	//Run the program based on the selected scheduling policy (need to do policy checks in interpreter's exec command)
	if (strcmp(policy, "FCFS") == 0)
		errorCode = FCFS(queue);
	// else if (strcmp(policy, "SJF") == 0)
	// 	errorCode = SJF(queue);
	// else if (strcmp(policy, "RR") == 0)
	// 	errorCode = RR(queue);
	// else
	// 	errorCode = AGING(queue);

	return errorCode;
}



//Stores file's code to shell memory, creates PCB, and 
int process_file(char *file, struct ReadyQueue *queue) {

	int errorCode = 0;

	//Save whole file's program to shell memory and get its location
	//If we run out of memory, save_to_memory handles that and exits back into the mysh shell for user to retry
	int *first_last = malloc(2*sizeof(int));	//NOTE: not sure if this pointer stuff is correct
	save_to_memory(file, first_last);

	//Set up PCB for that program
	struct PCB *pcb = (struct PCB*) malloc(sizeof(struct PCB));
	pcb->start_location = first_last[0];
	pcb->end_location = first_last[1];
	pcb->current_location = 0; 
	pcb->next = NULL;
	//TODO: Need to set PID, how do we go about this? Just increment a counter?
	
	//Add it to ready queue as it's good to go
	add_to_queue(pcb, queue);

	return errorCode;
}

//Saves the file's contents into shell memory
//Returns start_location and end_location to create the PCB
int* save_to_memory(char *file, int *first_last) {

    int errCode = 0;
	int position = -1;
	char line[1000];

	//Open file containing program
	FILE *p = fopen(file, "rt"); 
	if(p == NULL)
		 badcommandFileDoesNotExist();

	//Record start location
	fgets(line,999,p);
	int first = mem_set_line(line);
	if (first == -1) notEnoughMemory(); 
	first_last[0] = first;
	
	//Add whole program to shell memory
	while(1) {
		fgets(line,999,p);
		position = mem_set_line(line);
		if (position == -1) notEnoughMemory(); 

		if(feof(p)){
			//Record  index of last script line
			first_last[1] = position;
			break;
		}
	}

    fclose(p);
	return 0;
}



//SCHEDULING POLICIES
int FCFS(struct ReadyQueue *queue) {
	while (!queue->head) {
		struct PCB *pcb = pop_off_queue(queue);
		cpu_run(pcb->start_location, pcb->end_location);
		clear_prog(pcb->start_location, pcb->end_location);
		free(pcb);
	}
}
// int SJF(struct ReadyQueue *queue);
// int RR(struct ReadyQueue *queue);
// int AGING(struct ReadyQueue *queue);

//Clear the program from memory by deleting all lines from the shell memory
void clear_prog(int start_location, int end_location) {
	for (int i = start_location; i < end_location; i++) 
		return; //Need to clear memory somehow; do we just delete the contents or how do we go about this?
}


//Error messages and handling of them
int notEnoughMemory() {
	printf("%s\n", "Not enough memory"); 
	//TODO: clear shell memory and the PCB queues and PCBs here
	return 5;
}
int sameFileNames() {
	printf("%s\n", "Same file inputted twice"); 
	return 6;
}