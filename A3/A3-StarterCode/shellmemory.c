#include<stdlib.h>
#include<string.h>
#include<stdio.h>


struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct framestore[900];
struct memory_struct variablestore[100];

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

void mem_init(){

	int i;
	for (i=0; i<900; i++){		
		if (i<100) {
			variablestore[i].var = "none";
			variablestore[i].value = "none";
		}
		
		framestore[i].var = "none";
		framestore[i].value = "none";
	}

}

// Set key value pair
void mem_set_value_variable(char *var_in, char *value_in) {
	int i;

	for (i=0; i<1000; i++){
		if (strcmp(variablestore[i].var, var_in) == 0){
			variablestore[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<1000; i++){
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
	for (i=0; i<1000; i++){
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



//get value based on input key
char *mem_get_value_variable(char *var_in) {
	int i;

	for (i=0; i<1000; i++){
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
	for(int i = 0; i<100; i++) {
		variablestore[i].value = "none";
		variablestore[i].var = "none";
	}
}