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
	int size;
	//To know where the process is when going back to it after switching (used in RR)
	int current_location;
	//Queue stuff
	struct PCB *next;

};
//Ready queue
struct ReadyQueue {
	struct PCB *head;
	struct PCB *tail;
};



//Global vars
struct ReadyQueue queue = {.head = NULL, .tail = NULL};
int counter = 0;



int kernel(char *file1, char *file2, char *file3, char *policy);
int save_to_memory(char *file, int *first_last);
int process_file(char *file,char* policy);
void add_to_queue(struct PCB *pcb, char* policy);
struct PCB* pop_off_queue();
int notEnoughMemory();
int sameFileNames();
//List of policies
int FCFS_SJF();
int RR();
// int AGING();



//kernel method handles all kernel logic
//Returns 0 if all went well
//Returns -1 if something went wrong
int kernel(char *file1, char *file2, char *file3, char *policy) {

    int errorCode = 0;
	
	//Input checking of files
    //If file1 empty, no files at all so bad command
	//Add all programs to shell memory, create their PCBs, and add them to the ready queue
    if (!file1)
        return badcommand();
	errorCode = process_file(file1, policy);
	if (errorCode) return errorCode;

	//If any of the files are identical, wrong inputs as need to have unique for each. This works because 
	//we always fill in file1 before file2 and file2 before file3
	if (file1 && file2) {
		if (strcmp(file1, file2) == 0) return sameFileNames();
		errorCode = process_file(file2, policy);
		if (errorCode) return errorCode;
	}
	if (file3) {
		if (strcmp(file1, file3) == 0 || strcmp(file2, file3) == 0) return sameFileNames();
		errorCode = process_file(file3, policy);
		if (errorCode) return errorCode;
	}

	//Run the program based on the selected scheduling policy (need to do policy checks in interpreter's exec command)
	if (strcmp(policy, "FCFS") == 0 || strcmp(policy, "SJF") == 0)
		errorCode = FCFS_SJF();
	else if (strcmp(policy, "RR") == 0)
		errorCode = RR();
	// else
	// 	errorCode = AGING(queue);

	return errorCode;
}



//Stores file's code to shell memory, creates PCB, and 
int process_file(char *file, char *policy) {

	int errorCode = 0;

	//Save whole file's program to shell memory and get its location
	//If we run out of memory, save_to_memory handles that and exits back into the mysh shell for user to retry
	int *first_last = malloc(2*sizeof(int));	//NOTE: not sure if this pointer stuff is correct
	errorCode = save_to_memory(file, first_last);

	if(errorCode) return errorCode;

	//Set up PCB for that program
	struct PCB *pcb = (struct PCB*) malloc(sizeof(struct PCB));
	pcb->start_location = first_last[0];
	pcb->end_location = first_last[1];
	pcb->current_location = pcb->start_location; 
	pcb->next = NULL;
	pcb->PID = counter++;
	pcb->size = pcb->end_location - pcb->start_location + 1;
	
	//Add it to ready queue as it's good to go
	add_to_queue(pcb, policy);

	return errorCode;
}

//Saves the file's contents into shell memory
int save_to_memory(char *file, int *first_last) {

    int errCode = 0;
	char line[1000];

	//Open file containing program
	FILE *p = fopen(file, "rt"); 
	if(p == NULL)
		badcommandFileDoesNotExist();

	//Record start location
	fgets(line,999,p);
	int position = mem_set_line(line);
	if (position == -1) return notEnoughMemory(); 
	first_last[0] = position;
	
	//Add whole program to shell memory
	while(1) {

		if(feof(p)){
			//Record  index of last script line
			first_last[1] = position;
			break;
		}

		fgets(line,999,p);
		position = mem_set_line(line);
		if (position == -1) return notEnoughMemory(); 

		
	}

    fclose(p);
	return 0;
}



//SCHEDULING POLICIES
int FCFS_SJF() {
	struct PCB *pcb;
	while ((pcb = pop_off_queue())) {
		cpu_run(pcb->start_location, pcb->end_location);
		clearMemory(pcb->start_location, pcb->end_location);
		free(pcb);
	}
	return 0;
}
int RR() {
	struct PCB *pcb;
	int cur_location = -1;
	while ((pcb = pop_off_queue())) {
		cur_location = cpu_run_lines(pcb->current_location, pcb->end_location, 2);
		//If not done processing file, add it back into the queue
		if (cur_location != -1) {
			pcb->current_location = cur_location;
			pcb->next = NULL;
			add_to_queue(pcb, "");
		} else {
			clearMemory(pcb->start_location, pcb->end_location);
			free(pcb);
		}
	}
	return 0;
}
// int AGING();



//Queue functions
//Only queue actions are pop & add
void add_to_queue(struct PCB *pcb,char *policy) {
	//If it's the first element, it's the head and tail
	if (!queue.head) {
		queue.head = pcb;
		queue.tail = pcb;
	}
	else {
		if (strcmp(policy,"SJF") == 0) {
			//If it's SJF policy, we put the pcb at the right place in the queue
			struct PCB *head = queue.head; 

			if(pcb->size < head->size) {
				pcb->next = head;
				queue.head = pcb;
				return;
			}
			while (head->next) {
				if(pcb->size < head->next->size) {
					pcb->next = head->next;
					head->next = pcb;
					return;
				}
				head = head->next;
			}
		}

		//New tail
		queue.tail->next = pcb;
		queue.tail = pcb;
	} 
	
	
	
}

struct PCB* pop_off_queue() {
	//If last to pop, set head to NULL
	if (queue.head) {
		struct PCB *res = queue.head;
		queue.head = res->next;
		return res;
	}
	return NULL;
}



//Error messages and handling of them
int notEnoughMemory() {
	printf("%s\n", "Not enough memory"); 
	//TODO: clear shell memory and the PCB queues and PCBs here (using clearMemory)
	return 5;
}
int sameFileNames() {
	printf("%s\n", "Same file inputted twice"); 
	return 6;
}