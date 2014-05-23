/*
 * globalVariables.h
 *
 *  Created on: 22 maj 2014
 *      Author: przemek
 */

#ifndef GLOBALVARIABLES_H_
#define GLOBALVARIABLES_H_

// pojedynczy proces
typedef struct process {
	struct process * next;
	char **argv;
	pid_t pid;
	char completed;
	char stopped;
	int status;
} process;

// grupa procesów połączonych pipeami
typedef struct job {
	struct job * next;
	char * command;
	process * first_process;
	pid_t pgid;
	char notified;
	struct termios tmodes;
	int stdin;
	int stdout;
	int stderr;
} job;

/* Nazwa użytkownika */
char * userName;
/* Nazwa maszyny */
char * mashineName;
/* Akutalna relatywna scieżka do HOME */
char * actualDir;
/* Maksymalna wielkość inputu */
size_t maxUserInputSize;
/* Wejscie pobierane od użytkownika */
char * inputString;
/* Numer grupy procesów shella */
pid_t shell_pgid;
/* Początkowe ustawienia terminala */
struct termios shell_tmodes;
/* Deskryptor terminala */
int shell_terminal;
/* Czy shell działa w trybie interaktywnym */
int is_shell_interactive;
/* Lista jobów */
job * first_job;


#endif /* GLOBALVARIABLES_H_ */
