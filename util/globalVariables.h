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
	int foreground;
	int stdin;
	int stdout;
	int stderr;
} job;

/* Nazwa użytkownika */
extern char * userName;
/* Nazwa maszyny */
extern char * mashineName;
/* Akutalna relatywna scieżka do HOME */
extern char * actualDir;
/* Maksymalna wielkość inputu */
extern size_t maxUserInputSize;
/* Wejscie pobierane od użytkownika */
extern char * inputString;
/* Numer grupy procesów shella */
extern pid_t shell_pgid;
/* Początkowe ustawienia terminala */
extern struct termios shell_tmodes;
/* Deskryptor terminala */
extern int shell_terminal;
/* Czy shell działa w trybie interaktywnym */
extern int is_shell_interactive;
/* Lista jobów */
extern job * first_job;
/* Mapa zmiennych shella */
extern std::map<std::string,std::string> variables;

#endif /* GLOBALVARIABLES_H_ */
