CC = g++
CFLAGS= -c -Wall -pedantic -I $(CURDIR) -std=c++0x

all: shell parser_o

shell: main.o ./util/initialization.o ./processManagement/processManagement.o ./variables/variables.o ./commands/commands.o ./parser/parser.o ./interpreter/interpreter.o
	$(CC) $(LFAGS) main.o ./util/initialization.o ./processManagement/processManagement.o ./variables/variables.o ./commands/commands.o ./parser/parser.o ./interpreter/interpreter.o -o shell
	
main.o: main.cpp ./util/initialization.cpp ./util/initialization.h ./util/globalVariables.h ./processManagement/processManagement.cpp ./processManagement/processManagement.h ./variables/variables.cpp ./variables/variables.h ./commands/commands.cpp ./commands/commands.h ./parser/parser.cpp ./parser/parser.h ./interpreter/interpreter.cpp ./interpreter/interpreter.h
	$(CC) $(CFLAGS) main.cpp -o main.o
	
./util/initialization.o: ./util/initialization.cpp ./util/initialization.h ./util/globalVariables.h
	$(CC) $(CFLAGS) ./util/initialization.cpp -o ./util/initialization.o
	
./processManagement/processManagement.o: ./processManagement/processManagement.cpp ./processManagement/processManagement.h ./util/globalVariables.h
	$(CC) $(CFLAGS) ./processManagement/processManagement.cpp -o ./processManagement/processManagement.o
	
./variables/variables.o: ./variables/variables.cpp ./variables/variables.h ./util/globalVariables.h
	$(CC) $(CFLAGS) ./variables/variables.cpp -o ./variables/variables.o
	
./commands/commands.o: ./commands/commands.cpp ./commands/commands.h ./util/globalVariables.h
	$(CC) $(CFLAGS) ./commands/commands.cpp -o ./commands/commands.o
	
./parser/parser.o: ./parser/parser.cpp ./parser/parser.h ./util/globalVariables.h
	$(CC) $(CFLAGS) ./parser/parser.cpp -o ./parser/parser.o
	
./interpreter/interpreter.o: ./interpreter/interpreter.cpp ./interpreter/interpreter.h ./parser/parser.cpp ./parser/parser.h
	$(CC) $(CFLAGS) ./interpreter/interpreter.cpp -o ./interpreter/interpreter.o
	
parser_o: parser.o ./util/initialization.o ./processManagement/processManagement.o ./variables/variables.o ./commands/commands.o
	$(CC) $(LFAGS) parser.o ./util/initialization.o ./processManagement/processManagement.o ./variables/variables.o ./commands/commands.o -o parser_o
	
parser.o: parser.cpp ./util/initialization.cpp ./util/initialization.h ./util/globalVariables.h ./processManagement/processManagement.cpp ./processManagement/processManagement.h ./variables/variables.cpp ./variables/variables.h ./commands/commands.cpp ./commands/commands.h
	$(CC) $(CFLAGS) parser.cpp -o parser.o -std=c++11
	
cleanObjects:
	find . -type f -name '*.o' -delete
	
clean:
	find . -type f -name '*.o' -delete
	rm -f shell
	rm -f parser_o
