void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
int mem_set_line(char *scriptCode);
//2.2.1
char *mem_get_value_by_index(int i);
int clearMemory(int beginning,int end);