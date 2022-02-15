mysh: shell.c interpreter.c shellmemory.c
	gcc -c shell.c interpreter.c shellmemory.c
	gcc -o mysh shell.o interpreter.o shellmemory.o

debug:
	gcc -g -o mysh shell.c interpreter.c shellmemory.c

clean: 
	rm mysh; rm *.o
