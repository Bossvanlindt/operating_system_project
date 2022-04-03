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
	//A3 stuff
	FILE* file;
	int fileCompleted;
	// int line_number;		//(Unnecessary as fgets keeps track of line number)
	int pagetable[framesize/3];
	//Keeps track of the next pagetable index to fill with the next frame
	int next_page_to_fill;
	int cur_page;
	int offset;
};
//Ready queue
struct ReadyQueue {
	struct PCB *head;
	struct PCB *tail;
};

struct Frame {
	int number;
	struct Frame* next;
};
struct FrameReadyQueue {
	struct Frame *head;
	struct Frame *tail;
};



//Global vars
struct ReadyQueue queue = {.head = NULL, .tail = NULL};
struct FrameReadyQueue LRUqueue = {.head = NULL, .tail = NULL};
int counter = 0;
int counter_file_name = 0;



int kernel(char *file1, char *file2, char *file3, char *policy);
int save_to_memory(char *file, int *first_last);
int process_file(char *file,char* policy);
void add_to_queue(struct PCB *pcb, char* policy);
struct PCB* pop_off_queue();
struct Frame* pop_off_queue_LRU();
void decrease_score();
void lowest_score();
int notEnoughMemory();
int sameFileNames();
//List of policies
int FCFS_SJF();
int RR();
int AGING();
//A3 functions
int RR_a3(char* file1, char* file2, char* file3);
int free_frame_via_LRU();
void process_files_a3(char* file1, char* file2, char* file3);
char* copy_to_backingstore(char* file);
struct PCB* create_PCB(char* file);
void load_to_framestore(struct PCB* pcb);


//kernel method handles all kernel logic
//Returns 0 if all went well
//Returns -1 if something went wrong
int kernel(char *file1, char *file2, char *file3, char *policy) {

    int errorCode = 0;
	
	//Input checking of files
    //If file1 empty, no files at all so bad command
    if (!file1)
        return badcommand();


	//A3 delegate all work for RR to RR_a3 function
	if(strcmp(policy, "RR") == 0) {
		errorCode = RR_a3(file1, file2, file3);
		return errorCode;
	}
	

	//If any of the files are identical, wrong inputs as need to have unique for each. This works because 
	//we always fill in file1 before file2 and file2 before file3
	if (file1 && file2) {
		if (strcmp(file1, file2) == 0) return sameFileNames();
	}
	if (file3) {
		if (strcmp(file1, file3) == 0 || strcmp(file2, file3) == 0) return sameFileNames();
	}


	//processing files
	//Add all programs to shell memory, create their PCBs, and add them to the ready queue
	errorCode = process_file(file1, policy);
	if (errorCode) return errorCode;

	if (file2) {
		errorCode = process_file(file2, policy);
		if (errorCode) return errorCode;
	}
	if (file3) {
		errorCode = process_file(file3, policy);
		if (errorCode) return errorCode;
	}
	
	//Run the program based on the selected scheduling policy (need to do policy checks in interpreter's exec command)
	if (strcmp(policy, "FCFS") == 0 || strcmp(policy, "SJF") == 0)
		errorCode = FCFS_SJF();
	else if (strcmp(policy, "RR") == 0)
		errorCode = RR();
	else if (strcmp(policy, "AGING") == 0)
		errorCode = AGING(queue);
	else return badcommand();

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
		clearMemoryLines(pcb->start_location, pcb->end_location);
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
			clearMemoryLines(pcb->start_location, pcb->end_location);
			free(pcb);
		}
	}
	return 0;
}

int AGING() {
	struct PCB *pcb = NULL;
	int cur_location = -1;

	while (queue.head) {
		cur_location = cpu_run_lines(queue.head->current_location, queue.head->end_location, 1);
		decrease_score();
		//Update current location if not reached the end yet
		if (cur_location != -1) {
			queue.head->current_location = cur_location;
		
		} else {
			//Getting rid of the pcb if it's done
			pcb = pop_off_queue();
			clearMemoryLines(pcb->start_location, pcb->end_location);
			free(pcb);
		}
		//Decreasing score
		if(queue.head) lowest_score();
	}

	return 0;
}

void lowest_score() {
 	struct PCB *cur = queue.head;
 	struct PCB *min_pcb = queue.head;
	struct PCB *previous_min = NULL;
	struct PCB *previous = NULL;
 	int min = queue.head->size;

	//Until there's no pcb
 	while (cur) {
		 //Getting the minimum score
 		if (cur->size < min) {
 			min = cur->size;
 			min_pcb = cur;
			previous_min = previous;
 		}
		previous = cur;
 		cur = cur->next;
		
 	}

	if(min_pcb != queue.head) {
		//Rearranging queue list
		previous_min->next = min_pcb->next;
		struct PCB *old_head = pop_off_queue();
		add_to_queue(old_head,"");

		min_pcb->next = queue.head;
		queue.head = min_pcb;
	}
 }

void decrease_score() {
	//Decreasing every score except head
	struct PCB *cur = queue.head->next;
	while (cur) {
		if (cur->size > 0) cur->size--;
		cur = cur->next;
	}
}

//Queue functions
//Only queue actions are pop & add
void add_to_queue(struct PCB *pcb,char *policy) {
	//If it's the first element, it's the head and tail
	if (!queue.head) {
		queue.head = pcb;
		queue.tail = pcb;
	}
	else {
		if (strcmp(policy,"SJF") == 0 || strcmp(policy,"AGING") == 0) {
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
		queue.tail->next = NULL;
	} 
}

void add_to_LRU_queue(struct Frame *frame) {
	if (!LRUqueue.head) {
		LRUqueue.head = frame;
		LRUqueue.tail = frame;
		return;
	}
	//New tail
	LRUqueue.tail->next = frame;
	LRUqueue.tail = frame;
	LRUqueue.tail->next = NULL;
}

struct Frame* pop_off_queue_LRU() {
	//If last to pop, set head to NULL
	if (LRUqueue.head) {
		struct Frame *res = LRUqueue.head;
		LRUqueue.head = res->next;
		return res;
	}
	//This should never happen
	return NULL;
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


//A3 functions
int RR_a3(char* file1, char* file2, char* file3) {

	int errorCode = 0;
	//Overview
	//1. Copy each file to the backingStore directory
	//2. Create a PCB for each file, comes with file, line_number, pagetable, cur_page, offset
	//3. Add each PCB to the queue (necessary for round robin)
	//4. Start off by loading two frames per file into framestore
	//5. Run just like RR above but based on paging and handling of page faults

	//Steps 1 2 3 4
	process_files_a3(file1, file2, file3);

	//5. Run just like RR above but based on paging and frame*3 logic
	struct PCB *pcb;
	char* command;
	int ran_every_line;
	while ((pcb = pop_off_queue())) {
		//Keep track of whether we're done with every line of the file
		//NOTE: with this approach, if correct, we would not need the PCB fileCompleted variable anymore
		ran_every_line = 0;

		//Run two lines at a time
		for (int i = 0; i < 2; i++) {
			//Handle page fault by clearing a frame and loading the code to it (which is all handled by load_to_framestore)
			//Following the directions, if there is a page fault we do not run the next line but instead push the PCB to the back of the queue
			//and only load it into the frame. Continue with next one. 
			if (pcb->pagetable[pcb->cur_page] == -1) {
				//If file was completed before loading into framestore, then the process is done
				if (pcb->fileCompleted) ran_every_line = 1;
				load_to_framestore(pcb);
				break;
			}

			//Process line
			command = mem_get_value_by_index(pcb->pagetable[pcb->cur_page]*3 + pcb->offset);
			//If the command is none, we're done. Skip to the end to free the pcb and proceed with the next one. 
			if (strcmp(command, "none") == 0) {
				ran_every_line = 1;
				break;
			}

			//Run the command (no need to call cpu_run_lines as we're just running that command and we know it's all good)
			if (strchr(command, ';') != NULL) {
				char *parsedCommand = strtok(command, ";");	
				while (parsedCommand != NULL) {
					parseInput(parsedCommand);
					parsedCommand = strtok(NULL, ";");
				}
			//1.2.5 else just process that line normally
			} else parseInput(command);
			


			//Update pcb pointers
			//If end of that page, go to next
			if (pcb->offset == 2) {
				//Create frame struct for the specific done frame 
				struct Frame *frame = malloc(sizeof(struct Frame));
				frame->number = pcb->pagetable[pcb->cur_page];

				//add it to the queue
				add_to_LRU_queue(frame);

				//Go to next page (via modulo logic to loop around)
				pcb->cur_page = (pcb->cur_page + 1) % (framesize/3);
				pcb->offset = 0;
			} else {
				pcb->offset++;
			}

		}
		
		//Skip to here if we're at the end of the file, so we don't end up trying to parseInput("none")
		//If not done processing file, add it back into the queue
		if (ran_every_line == 0) {
			pcb->next = NULL;
			add_to_queue(pcb, "");
		} else {
			//No need to clear memory lines here as that is done progressively as we finish each frame
			free(pcb);
		}
	}

	return 0;
}

void process_files_a3(char* file1, char* file2, char* file3) {
	struct PCB *pcb = NULL;
	char *str;

	//newName updated after calling this function
	str = copy_to_backingstore(file1);
	pcb = create_PCB(str);			//(Changed to newName instead of file1 as we now read from backingStore)
	add_to_queue(pcb, "RR");			//(Changed input to pcb instead of pcb.head, I think pcb.head is incorrect)
	//Loads 2 frames for that file
	load_to_framestore(pcb);
	load_to_framestore(pcb);

	if (file2) {
		str = copy_to_backingstore(file2);
		pcb = create_PCB(str);
		add_to_queue(pcb, "RR");
		load_to_framestore(pcb);
		load_to_framestore(pcb);
	}
	if (file3) {
		str = copy_to_backingstore(file3);
		pcb = create_PCB(str);
		add_to_queue(pcb, "RR");
		load_to_framestore(pcb);
		load_to_framestore(pcb);
	}
}

char* copy_to_backingstore(char *file) {	
	//Increment counter_file_name for future calls to avoid duplicate names
	char str[strlen("./backingStore/") + 4];	//4 for number of digits in counter 
	sprintf(str, "./backingStore/%d", counter);

	FILE *newFile = fopen(str, "w");
	FILE *oldFile = fopen(file, "r");

	char c;
	while ((c = fgetc(oldFile)) != EOF) {
		fputc(c, newFile);
	}

	fclose(newFile);
	fclose(oldFile);

	return strdup(str);
}

//Creates a PCB for the given file. Returns a pointer to the PCB
struct PCB* create_PCB(char *file) {
	struct PCB *pcb = malloc(sizeof(struct PCB));
	pcb->PID = counter++;
	pcb->next = NULL;
	pcb->file = fopen(file, "r");
	pcb->fileCompleted = 0;
	// pcb->line_number = 0;		//(No need since fgets keeps track of the line we're at & we keep the file open)
	//Initialize pagetable to all -1 as it doesn't contain any entries yet
	for (int i = 0; i < framesize/3; i++) {
		pcb->pagetable[i] = -1;
	}
	pcb->next_page_to_fill = 0;
	pcb->cur_page = 0;
	pcb->offset = 0;
	//Seems like we don't need these details
	// pcb->size = get_file_size(file);
	// pcb->start_location = 0;
	// pcb->end_location = 0;

	return pcb;
}

void load_to_framestore(struct PCB* pcb) {
	//Checks if file is already fully loaded
	//TODO: do we need to still fill the next frame as "none" to notify that we're at the end of the file?
	if (pcb->fileCompleted) return;
	
	int frameNumber = available_frame();

	
	//if frameStore full, handle page fault
	if (frameNumber == -1) frameNumber = free_frame_via_LRU();
		
	char line[1000];

	//Loads lines and checks if it's the end of the file
	for (int i = 0; i < 3; i++) {
		fgets(line, 999, pcb->file);
		mem_set_line_by_frame(line, frameNumber);

		if (feof(pcb->file)) {
			pcb->fileCompleted = 1;
			fclose(pcb->file);
			break;
		}
	}

	//Update pagetable
	pcb->pagetable[pcb->next_page_to_fill] = frameNumber;
	//Loop around once pagetable is full via modulo logic
	pcb->next_page_to_fill = (pcb->next_page_to_fill + 1) % (framesize/3);
}


//Based on LRU, finds and clears a frame. Returns the frame number of the cleared frame. 
int free_frame_via_LRU() {
	//Get the first frameNumber from the queue, i.e. the LRU
	struct Frame *frame = pop_off_queue_LRU();
	int frameNumber = frame->number;

	//Print the frame's contents
	printf("Page fault! Victim page contents:\n");
	printf("%s", mem_get_value_by_index(frameNumber*3));
	printf("%s", mem_get_value_by_index(frameNumber*3+1));
	printf("%s", mem_get_value_by_index(frameNumber*3+2));
	printf("End of victim page contents.\n");

	//Free the frame in the memory
	free_frame(frameNumber);
	free(frame);

	return frameNumber;
}