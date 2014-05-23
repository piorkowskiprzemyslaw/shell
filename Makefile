CC = g++
CFLAGS= -c -Wall -pedantic

all: shell

shell: main.o
	$(CC) $(LFAGS) main.o -o shell
	
main.o: main.cpp initialization.h globalVariables.h processManagement.h
	$(CC) $(CFLAGS) main.cpp -o main.o
	
cleanObjects:
	find . -type f -name '*.o' -delete
	
clean:
	find . -type f -name '*.o' -delete
	rm shell