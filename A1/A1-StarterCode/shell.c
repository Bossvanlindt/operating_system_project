
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>

#include "interpreter.h"
#include "shellmemory.h"


int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);

// Start of everything
int main(int argc, char *argv[]) {

	printf("%s\n", "Shell version 1.1 Created January 2022");
	help();

	char prompt = '$';  				// Shell prompt
	char userInput[MAX_USER_INPUT];		// user's input stored here
	int errorCode = 0;					// zero means no error, default

	//init user input
	for (int i=0; i<MAX_USER_INPUT; i++)
		userInput[i] = '\0';
	
	//init shell memory
	mem_init();
	//1.2.3 stdin is the file pointer
	FILE *input = stdin;

	while(1) {				
		//1.2.3 if the terminal is the input file pointer we print $
		//Compares input's file descriptor with the terminal's
		if (isatty(fileno(input))) {
			printf("%c ",prompt);
			
		}	
		//1.2.3 Runs on stdin as long as it doesn't return NULL
		//1.2.3 Happens when a file is redirected
		if (fgets(userInput, MAX_USER_INPUT-1, input) == NULL) {
			//1.2.3 If we reached EOF, then we switch the input to terminal file process
			input = fopen("/dev/tty","r");
			//1.2.3 If failed
			if(!input) {
				perror("Error from the terminal");
				exit(1);
			}
			continue;
		}

		errorCode = parseInput(userInput);
		if (errorCode == -1) exit(99);	// ignore all other errors
		memset(userInput, 0, sizeof(userInput));

	}

	return 0;

}

// Extract words from the input then call interpreter
int parseInput(char ui[]) {
 
	char tmp[200];
	char *words[100];							
	int a,b;							
	int w=0; // wordID

	for(a=0; ui[a]==' ' && a<1000; a++);		// skip white spaces

	while(ui[a] != '\0' && a<1000) {

		for(b=0; ui[a]!='\0' && ui[a]!=' ' && a<1000; a++, b++)
			tmp[b] = ui[a];						// extract a word
	 
		if (tmp[b-1] == '\n') tmp[b-1] = '\0';//1.2.3 remove line break for commands in files
		else tmp[b] = '\0';

		words[w] = strdup(tmp);

		a++; 
		w++;
	}

	return interpreter(words, w);
}
