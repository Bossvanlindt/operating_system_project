#include<stdlib.h>
#include<string.h>
#include<stdio.h>


struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct framestore[framesize];
struct memory_struct variablestore[varmemsize];

// Helper functions
int match(char *model, char *var) {
	int i, len=strlen(var), matchCount=0;
	for(i=0;i<len;i++)
		if (*(model+i) == *(var+i)) matchCount++;
	if (matchCount == len)
		return 1;
	else
		return 0;
}

char *extract(char *model) {
	char token='=';    // look for this to find value
	char value[1000];  // stores the extract value
	int i,j, len=strlen(model);
	for(i=0;i<len && *(model+i)!=token;i++); // loop till we get there
	// extract the value
	for(i=i+1,j=0;i<len;i++,j++) value[j]=*(model+i);
	value[j]='\0';
	return strdup(value);
}


// Shell memory functions

void mem_init() {
	for (int i = 0; i < varmemsize; i++) {
		variablestore[i].var = "none";
		variablestore[i].value = "none";
	}

	for (int i = 0; i < framesize; i++) {		
		framestore[i].var = "none";
		framestore[i].value = "none";
	}
	
}

// Set key value pair
void mem_set_value_variable(char *var_in, char *value_in) {
	int i;

	for (i=0; i<varmemsize; i++){
		if (strcmp(variablestore[i].var, var_in) == 0){
			variablestore[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<varmemsize; i++){
		if (strcmp(variablestore[i].var, "none") == 0){
			variablestore[i].var = strdup(var_in);
			variablestore[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

//2.2.1 saves script in memory
int mem_set_line(char *scriptCode) {
	int i;

	//Value does not exist, need to find a free spot.
	for (i=0; i<framesize; i++){
		if (strcmp(framestore[i].var, "none") == 0){
			framestore[i].var = "";
			framestore[i].value = strdup(scriptCode);
			//Returns position in memory
			return i;
		} 
	}
	//Error no more memory
	return -1;

}

//Copies scriptCode into the first free line in the frame indexed at frameNumber
void mem_set_line_by_frame(char *scriptCode, int frameNumber) {
	//Value does not exist, need to find a free spot.
	for (int i = frameNumber*3; i < frameNumber*3 + 3; i++) {
		if (strcmp(framestore[i].var, "none") == 0) {
			framestore[i].var = "";
			framestore[i].value = strdup(scriptCode);
			return;
		}
	}
}

void free_frame(int frameNumber) {
	
	for(int i=frameNumber*3;i < frameNumber*3 + 3;i++) {
		framestore[i].var = "none";
		framestore[i].value = "none";
	}
}

//Returns first available frame
int available_frame() {
	for(int i = 0; i < framesize; i += 3) {
		if (strcmp(framestore[i].var, "none") == 0) {
			return i/3;
		}
	}
	//returns -1 if no frame available
	return -1;
}



//get value based on input key
char *mem_get_value_variable(char *var_in) {
	int i;

	for (i=0; i<varmemsize; i++){
		if (strcmp(variablestore[i].var, var_in) == 0){

			return strdup(variablestore[i].value);
		} 
	}
	return "Variable does not exist";

}

//2.2.1
//Get value based on index
char *mem_get_value_by_index(int i) {
	return strdup(framestore[i].value);
}
//Clear shell memory of the saved programs when using exec
void clearMemoryLines(int beginning, int end) {
	for(int i = beginning; i<=end; i++) {
		framestore[i].value = "none";
		framestore[i].var = "none";
	}
}
//Resets variables
void resetVariables() {
	for(int i = 0; i<varmemsize; i++) {
		variablestore[i].value = "none";
		variablestore[i].var = "none";
	}
}