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

#include "globalVariables.h"
#include "initialization.h"
#include "processManagement.h"

/**
 * To jest do wywalenia, pokazuje tylko jak korzystać z tego zarządzania procesami.
 */
void exampleRun(char * arg)
{
	//int pfile = open("outputTest.txt", O_CREAT | O_RDWR , S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR );
	process * p = ( process * ) malloc( sizeof(process) );
	job * j = ( job * ) malloc( sizeof(job) );
	j->command = arg;
	j->first_process = p;
	j->stdin = STDIN_FILENO;
	j->stdout = STDOUT_FILENO; /* może być też pfile jeśli chcemy zrobić przekierwoanie do pliku. */
	j->stderr = STDERR_FILENO;
	p->next = NULL;

	// usunięcie niepotrzebnego znaku nowej linii.
	for( int i = 0 ; arg[i] ; ++i ) {
		if( arg[i] == '\n' )
			arg[i] = '\0';
	}

	char * execArgs[] = { arg, NULL };
	p->argv = execArgs;

	// jeśli proces ma być w background to drugim argumentem jest 0.
	launch_job(j, 1);
}

int main(void)
{
	char *inputString;
	size_t maxUserInputSize;

	init();

	while( true ) {
		refreshVariables();
		printf("%s@%s:~%s$ ", userName, mashineName, actualDir);
		fflush(stdin);
		bzero(inputString, maxUserInputSize);
		getline(&inputString, &maxUserInputSize, stdin);

		if( strcmp(inputString, "exit\n") == 0 ) {
			break;
		}

		if( strcmp(inputString, "\n") != 0 )
			exampleRun(inputString);
	}

}

