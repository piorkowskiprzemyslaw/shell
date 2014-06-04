CC = g++
CFLAGS= -c -Wall -pedantic -I $(CURDIR)

all: shell

shell: main.o ./util/initialization.o ./processManagement/processManagement.o ./variables/variables.o ./commands/commands.o
	$(CC) $(LFAGS) main.o ./util/initialization.o ./processManagement/processManagement.o ./variables/variables.o ./commands/commands.o -o shell
	
main.o: main.cpp ./util/initialization.cpp ./util/initialization.h ./util/globalVariables.h ./processManagement/processManagement.cpp ./processManagement/processManagement.h ./variables/variables.cpp ./variables/variables.h ./commands/commands.cpp ./commands/commands.h
	$(CC) $(CFLAGS) main.cpp -o main.o
	
./util/initialization.o: ./util/initialization.cpp ./util/initialization.h ./util/globalVariables.h
	$(CC) $(CFLAGS) ./util/initialization.cpp -o ./util/initialization.o
	
./processManagement/processManagement.o: ./processManagement/processManagement.cpp ./processManagement/processManagement.h ./util/globalVariables.h
	$(CC) $(CFLAGS) ./processManagement/processManagement.cpp -o ./processManagement/processManagement.o
	
./variables/variables.o: ./variables/variables.cpp ./variables/variables.h ./util/globalVariables.h
	$(CC) $(CFLAGS) ./variables/variables.cpp -o ./variables/variables.o
	
./commands/commands.o: ./commands/commands.cpp ./commands/commands.h ./util/globalVariables.h
	$(CC) $(CFLAGS) ./commands/commands.cpp -o ./commands/commands.o
	
cleanObjects:
	find . -type f -name '*.o' -delete
	
clean:
	find . -type f -name '*.o' -delete
	rm shell
