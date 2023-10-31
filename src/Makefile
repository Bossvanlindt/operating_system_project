mysh: shell.c interpreter.c shellmemory.c
	gcc -D framesize=$(framesize) -D varmemsize=$(varmemsize) -c shell.c interpreter.c shellmemory.c kernel.c cpu.c
	gcc -o mysh shell.o interpreter.o shellmemory.o kernel.o cpu.o

debug:
	gcc -D framesize=$(framesize) -D varmemsize=$(varmemsize) -g -o mysh shell.c interpreter.c shellmemory.c kernel.c cpu.c

clean: 
	rm mysh; rm *.o
