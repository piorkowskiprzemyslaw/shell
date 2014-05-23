CC = g++
CFLAGS= -c -Wall -pedantic -I $(CURDIR)

all: shell

shell: main.o ./util/initialization.o ./processManagement/processManagement.o
	$(CC) $(LFAGS) main.o ./util/initialization.o ./processManagement/processManagement.o -o shell
	
main.o: main.cpp ./util/initialization.cpp ./util/initialization.h ./util/globalVariables.h ./processManagement/processManagement.cpp ./processManagement/processManagement.h
	$(CC) $(CFLAGS) main.cpp -o main.o
	
./util/initialization.o: ./util/initialization.cpp ./util/initialization.h ./util/globalVariables.h
	$(CC) $(CFLAGS) ./util/initialization.cpp -o ./util/initialization.o
	
./processManagement/processManagement.o: ./processManagement/processManagement.cpp ./processManagement/processManagement.h ./util/globalVariables.h
	$(CC) $(CFLAGS) ./processManagement/processManagement.cpp -o ./processManagement/processManagement.o
	
cleanObjects:
	find . -type f -name '*.o' -delete
	
clean:
	find . -type f -name '*.o' -delete
	rm shell