#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <dirent.h>
#include "shellmemory.h"
#include "shell.h"
#include "kernel.h"

int MAX_ARGS_SIZE = 3;
//1.2.1 change to 7 for 5 tokens + "set" + variable name
int MAX_SET_SIZE = 7;
//1.2.4 file struct linked list
struct Files {
	char name[100];
	struct Files *next;
};




int help();
int quit();
int badcommand();
//1.2.1 changed argument to char* values[] and int values_size
int set(char* var, char* values[], int values_size);
int echo(char* var);
int print(char* var);
int my_ls();
int compareStrings(char* first, char* second);
struct Files* sortFile(struct Files *ptr, struct Files *file);
void freeMemory(struct Files *head);
int run(char* script);
int badcommandFileDoesNotExist();
//1.2.1 error if too many tokens
int badcommandTooManyTokens();





// Interpret commands and their arguments
int interpreter(char* command_args[], int args_size){
	int i;

	//1.2.1 adapt to make it work with special set tokens
	if ( args_size < 1 || args_size > MAX_ARGS_SIZE){
		if (strcmp(command_args[0], "set")==0){
			if (args_size > MAX_SET_SIZE)
				return badcommandTooManyTokens();
		} else
			return badcommand();
	}


	for ( i=0; i<args_size; i++){ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help")==0){
	    //help
	    if (args_size != 1) return badcommand();
	    return help();
	
	} else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand();
		return quit();

	} else if (strcmp(command_args[0], "set")==0) {
		//set
		//1.2.1 change arg_size to <= 7 to store up to 5 tokens 
		if (args_size > 7) return badcommandTooManyTokens();
		//1.2.1 Store all arguments at indices 2 or greater in an array
		char* to_store[args_size-2];
		for (i=2; i<args_size; i++){
			to_store[i-2] = command_args[i];
		}
		return set(command_args[1], to_store, args_size-2);
	
	} else if(strcmp(command_args[0], "echo") == 0) {
		//echo
		//1.2.2 only 1 argument
		if (args_size != 2) return badcommand();
		return echo(command_args[1]);

	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "my_ls") == 0) {
		if (args_size != 1) return badcommand();
		return my_ls();
	} 
	else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);
	
	} else return badcommand();
}

int help(){

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
echo $VAR/STRING	Displays the STRING assigned to VAR or displays the STRING\n \
print VAR		Displays the STRING assigned to VAR\n \
my_ls			Displays the files and directories in the current directory \n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

int quit(){
	printf("%s\n", "Bye!");
	exit(0);
}

int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;
}

// For run command only
int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
}

//1.2.1 error if too many tokens
int badcommandTooManyTokens(){
	printf("%s\n", "Bad command: Too many tokens");
	return 4;
}

//1.2.1 changed function arguments and adapted for up to 5 tokens
int set(char* var, char* values[], int values_size){

	//1.2.1 value has length 100 per word + 1 for each space in between
	char value[values_size*101];
	value[0] = '\0';
	for (int i=0; i<values_size; i++){
		strcat(value, values[i]);
		strcat(value, " ");
	}

	mem_set_value(var, value);

	return 0;

}
//1.2.2 new echo function
int echo(char *var) {
	if(var[0] == '$' ) {
		//1.2.2 Send the variable name starting after $ if it's a variable
		print(&var[1]);
	}
	else {
		printf("%s\n",var);
	}
	return 0;
}

int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}

int my_ls() {


	//1.2.4 Open current directory stream
	DIR *current_d = opendir(".");
	//1.2.4 Creating directory entry
	struct dirent *d;
	if (current_d == NULL) {
		printf("Can't open current directory");
		return 5;
	}

	struct Files *file, *head;
	int count = 0;
	//1.2.4 Reading every file and directory entry in current directory
	while ((d = readdir(current_d)) != NULL ) {
		//Ignoring current and parent directory
		if (strcmp(d->d_name,"..") == 0 || strcmp(d->d_name,".") == 0) {
			continue;
		}
		file = (struct Files*) malloc(sizeof(struct Files));
		//1.2.4 Saving the file/dir name
		strcpy(file->name,d->d_name);
		//1.2.4 If the file is the first found, then it's the head of the linked list
		if(count == 0) {
			head = file;
			file->next = NULL;
			count++;
			continue;
		}

		head = sortFile(head,file);
	}
	//1.2.4 displaying all the files alphabeticallt
	printf("%s\n",head->name);
	file = head;
	while(file->next != NULL) {
		printf("%s\n",file->next->name);
		file = file->next;
	}
	freeMemory(head);
	free(current_d);
	return 0;
}
//1.2.4 We want lowercase letters to be put before uppercase letters
int compareStrings(char* first, char* second) {
	//1.2.4 Checks which filename is greater than the other
	//1 means first is bigger, -1 means second is bigger
	for(int i=0; second[i] && first[i]; i++) {		
		int letter1 = first[i];
		int letter2 = second[i];
		//1.2.4 we compare lower and upper case letters the same
		if(first[i] >= 97 && first[i] <=122) {
			letter1 -= 32;
		}	
		if(second[i] >= 97 && second[i] <=122) {
			letter2 -= 32;
		}
		if (letter1 > letter2) {
			return 1;
		}
		else if(letter1 < letter2) {
			return -1;
		}
		//1.2.4 That means it's the same letter
		else {
			//1.2.4 This means first[i] is lowercase, so second is bigger
			if (first[i] > second[i]) {
				return -1;
			}
			//1.2.4 Vice-versa
			else if (first[i] < second[i]) {
				return 1;
			}
			//1.2.4 same letter
			else {
				continue;
			}
		}
	
	}
	//1.2.4 Impossible to get there as it would mean the same file name
	return 1;
}

struct Files* sortFile(struct Files *ptr, struct Files *file) {
	//1.2.4 If the file name is smaller than the head, the file variable is the new head
	if (compareStrings(ptr->name,file->name) == 1) {
		file->next = ptr;
		return file;
	}

	struct Files *head = ptr;
	//1.2.4 Iterating through the current file names
	while(ptr->next != NULL) {
		//1.2.4 If the file variable name is smaller we insert it before ptr->next and after ptr
		if (compareStrings(ptr->next->name,file->name) == 1) {
			file->next = ptr->next;
			ptr->next = file;
			return head;
		}
		ptr = ptr->next;
	}
	//1.2.4 if it's bigger than all the file names in the linked list, we put it at the end
	ptr->next = file;
	file->next = NULL;
	return head;
}
//1.2.4 Freeing the memory of the struct linked list
void freeMemory(struct Files *head) {
	struct Files *temp;
	//1.2.4 iterating through the linked list
	while (head != NULL) {
		temp = head;
		head = head->next;
		free(temp);
	}
}

//2.2.1 edited to work as a process by calling kernel function
int run(char* script){
	return kernel(script, NULL, NULL, "FCFS");
}