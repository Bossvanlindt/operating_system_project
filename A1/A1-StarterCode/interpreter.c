#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "shellmemory.h"
#include "shell.h"

int MAX_ARGS_SIZE = 3;
//1.2.1 change to 7 for 5 tokens + "set" + variable name
int MAX_SET_SIZE = 7;

int help();
int quit();
int badcommand();
//1.2.1 changed argument to char* values[] and int values_size
int set(char* var, char* values[], int values_size);
int print(char* var);
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
	
	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);
	
	} else return badcommand();
}

int help(){

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
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

	//Yann: Why is there this buffer stuff? Doesn't seem to do anything
	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);

	mem_set_value(var, value);

	return 0;

}

int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}

int run(char* script){
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script,"rt");  // the program is in a file

	if(p == NULL){
		return badcommandFileDoesNotExist();
	}

	fgets(line,999,p);
	while(1){
		errCode = parseInput(line);	// which calls interpreter()
		memset(line, 0, sizeof(line));

		if(feof(p)){
			break;
		}
		fgets(line,999,p);
	}

    fclose(p);

	return errCode;
}
