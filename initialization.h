/*
 * initialization.cpp
 *
 *  Created on: 22 maj 2014
 *      Author: przemek
 */

#ifndef INITIALIZATION_H_
#define INITIALIZATION_H_

#include <stdio.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include "globalVariables.h"
#include "processManagement.h"

/**
 * Inicjalizacja shella
 */
void init()
{
	first_job = NULL;
	maxUserInputSize = 128;
	inputString = (char*)malloc(sizeof(char) * maxUserInputSize);
	struct passwd * pass = getpwuid(getuid());
	mashineName = (char*)malloc(sizeof(char) * 64);
	actualDir = (char*)malloc(sizeof(char) *128);
	userName = pass->pw_name;
	gethostname(mashineName,100);
	shell_terminal = STDIN_FILENO;
	is_shell_interactive = isatty(shell_terminal);

	if(is_shell_interactive) {

		// jeśli jestesmy podlaczeni pod terminal, ale jestesmy w background to usypiamy
		while( tcgetpgrp(shell_terminal) != ( shell_pgid = getpgrp() ) ) {
			kill( -shell_pgid , SIGTTIN );
		}

		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(SIGTSTP, SIG_IGN);
		signal(SIGTTIN, SIG_IGN);
		signal(SIGTTOU, SIG_IGN);
		signal(SIGCHLD, SIG_IGN);

		shell_pgid = getpid();
		if( setpgid( shell_pgid, shell_pgid ) < 0 ) {
			perror("Nie mozna zarejestrowac shella w swojej wlasnej grupie procesow.");
		}

		//przejęcie terminala
		tcsetpgrp(shell_terminal, shell_pgid);
		//zapisanie domyslnych wartości ustawień terminala
		tcgetattr(shell_terminal, &shell_tmodes);
	}
}

/**
 * Obsługa SIGCHLD
 */
void signalHandler(int signo)
{
	do_job_notification();
}

/**
 * Zwraca aktualną ścieżke relatywną do HOME
 */
void refreshVariables()
{
	struct passwd * pass = getpwuid(getuid());
	mashineName = (char*)malloc(sizeof(char) * 64);
	userName = pass->pw_name;
	gethostname(mashineName,100);
	char * home = getenv("HOME");
	char fullDir[128];
	getcwd(fullDir, 128);
	strcpy(actualDir, fullDir + strlen(home));
}

#endif /* INITIALIZATION_H_ */
