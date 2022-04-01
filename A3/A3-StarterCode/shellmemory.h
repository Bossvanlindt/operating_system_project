void mem_init();
char *mem_get_value_variable(char *var);
void mem_set_value_variable(char *var, char *value);
int mem_set_line(char *scriptCode);
//2.2.1
char *mem_get_value_by_index(int i);
int clearMemoryLines(int beginning,int end);
void resetVariables(); 
int available_frame();
void mem_set_line_by_frame(char *scriptCode, int frameNumber);
void free_frame(int frameNumber);