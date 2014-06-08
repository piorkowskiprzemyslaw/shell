/*
 * main.cpp
 *
 *  Created on: 28 kwi 2014
 *      Author: przemek
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <map>
#include <string>

#include <util/globalVariables.h>
#include <util/initialization.h>
#include <processManagement/processManagement.h>
#include <variables/variables.h>
#include <commands/commands.h>
#include <parser/parser.h>
#include <interpreter/interpreter.h>

/**
 * To jest do wywalenia, pokazuje tylko jak korzystać z tego zarządzania procesami.
 */
void exampleRun(char * arg)
{
	//int pfile = open("outputTest.txt", O_CREAT | O_RDWR , S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR );
	process * p = ( process * ) malloc( sizeof(process) );
	job * j = ( job * ) malloc( sizeof(job) );
	j->pgid = 0 ;
	j->command = (char *)malloc(sizeof(char) * (strlen(arg) - 1));

	for(size_t i = 0 ; i + 1 < strlen(arg) ; ++i){
		j->command[i] = arg[i];
	}

	j->next = NULL;
	j->first_process = p;
	j->notified = 0;
	j->stdin = STDIN_FILENO;
	j->stdout = STDOUT_FILENO; /* może być też pfile jeśli chcemy zrobić przekierwoanie do pliku. */
	j->stderr = STDERR_FILENO;
	p->next = NULL;
	p->completed = 0;
	p->stopped = 0;
	p->pid = 0;
	p->status = 0;

	// usunięcie niepotrzebnego znaku nowej linii.
	for( int i = 0 ; arg[i] ; ++i ) {
		if( arg[i] == '\n' )
			arg[i] = '\0';
	}

	char * execArgs[] = { arg, NULL };
	p->argv = execArgs;

	job * actualJob;
	if(first_job != NULL) {
		for(actualJob = first_job ; actualJob->next ; actualJob = actualJob->next){ }
		actualJob->next = j;
	} else {
		first_job = j;
	}

	// jeśli proces ma być w background to drugim argumentem jest 0.
	launch_job(j, 1);
}

int main(void)
{
	init();

	while( true ) {
		parser::REDIRECTION tokens;
		refreshVariables();
		do_job_notification();
		printf("%s@%s:%s# ", userName, mashineName, actualDir);
		fflush(stdin);
		memset(inputString, 0, maxUserInputSize);
		getline(&inputString, &maxUserInputSize, stdin);

		if( strcmp(inputString, "exit\n") == 0 ) {
			break;
		}

		if( strcmp(inputString, "\n") != 0 && parser::parse(inputString,tokens) ) {
			interpreter::mini_shell_printer printer;
			printer(tokens);
		}

	}
}

