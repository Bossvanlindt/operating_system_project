//2.2.1

int kernel(char *file1, char *file2, char *file3, char *policy);

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

