1.2.1
* Use random integer generating / hashing for unique PID
* Saving to shell memory - can have key be line number and value be the instruction so we can store line number in PCB and keep track of where we’re at
    * Can iterate through shell memory
* PCB struct w id, start int (where is it stored in shell memory), PC program counter (int for line we’re at/how many lines we’ve processed)
* CPU.c & CPU.h for CPU behaviour, cpu_run function fetches head and executes a couple of commands before pushing it back on the ready queue
    * Can have two, one that runs a couple commands at a time for Round Robin and one that runs it all for the other approaches
* Steps when calling run: load to memory, create PCB and push it to ready queue, run all tasks in queue
* Can create a ker.c file to simulate kernel, that takes care of queue, PCBs, and cpu_run function
    * Kernel function exits once queue empty
    * Consider having a scheduling policy argument, where FCFS is basically the default and doesn’t really matter when we have one program running using ‘run’ anyways

1.2.2
* Round robin: run prog1 2 lines, then prog2 2 lines, etc. in loop until done

1.2.3
* Could add scheduling policies inside ker.c, where last policy argument of kernel function dictates which policy is used
* Then, in exec command definition, we simply call kernel function with array containing each program info (max 3) as well as policy as second argument. Or just three first args are three programs, with NULL if no program so it’s easier and no arrays
* Scheduler part of kernel, function in kernel, one function per policy

1.2.4
* What about filling the memory with all the lines from each program? Need to free them afterwards, so that weird line-numbered vars aren’t accessible to user after exec finishes running. Maybe there’s a better way to do this though, not sure
