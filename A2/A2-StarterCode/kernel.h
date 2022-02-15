//2.2.1

int kernel(char *file1, char *file2, char *file3, char *policy);

//2.2.1 PCB struct
struct PCB {
	int PID;
	int start_location;
//	int firstLine; YOUNES: I'm not sure why we need this? Maybe it's not needed when we do this by index instead
	int end_location;
};

//2.2.1 ready queue
struct ReadyQueue {
	struct PCB *current;
	struct PCB *next;
};

//2.2.1 3 process at most
struct memory_struct shellmemory[3];