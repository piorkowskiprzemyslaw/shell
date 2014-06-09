
#include <processManagement/processManagement.h>

/**
 * Znajdź pracę o zadanym pgid
 */
job * find_job(pid_t pgid)
{
	job * j;
	for( j = first_job ; j ; j = j->next ) {
		if( j->pgid == pgid ) {
			return j;
		}
	}
	return NULL;
}

/**
 * Sprawdza czy wszystkie procesy w ramach pracy się zakończyły
 * lub czy są zastopowane.
 */
int job_is_stopped(job * j)
{
	process *p;
	for( p = j->first_process ; p ; p = p->next ) {
		if( !p->completed && !p->stopped) {
			return 0;
		}
	}
	return 1;
}

/**
 * Sprawdza czy wszystkie procesy w ramach pracy zostały zakończone.
 */
int job_is_completed(job * j)
{
	process *p;
	for( p = j->first_process ; p ; p = p->next ) {
		if( !p->completed ) {
			return 0;
		}
	}
	return 1;
}

/**
 * Wyświetl wiadomość dotyczącą pracy.
 */
void format_job_info( job *j, const char *status )
{
	if(!j->foreground) {
		fprintf(stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command );
	}
}

/**
 * Zwalnia zasoby zajmowane przez konkretną pracę.
 */
void free_job( job * j )
{
	process * p , * next;
	for( p = j->first_process ; p ;  ) {
		next = p->next;
		free(p);
		p = next;
	}
	free(j->command);
	free(j);
}

/**
 * Dla pid-u który został zwrócony przez waitpid sprawdź jego status
 * i przygotuj informacje dla użytkownika o szczegółach zamknięcia tego
 * pliku.
 */
int mark_process_status(pid_t pid, int status)
{
	job *j;
	process *p;


	if( pid > 0 ) {

		for( j = first_job ; j ; j = j->next ) {
			for( p = j->first_process ; p ; p = p->next ) {
				if( p->pid == pid ) {
					p->status = status;

					if( WIFSTOPPED(status) ) {
						p->stopped = 1;
					} else {
						p->completed = 1;
						if( WIFSIGNALED(status) ) {
							fprintf(stderr, "%d: terminated by signal %d.\n", (int) pid, WTERMSIG(p->status) );
						}
					}

				return 0;
				}
			}
		}

		fprintf(stderr, "No child process %d.\n", pid);
		return -1;
	} else if ( pid == 0 || errno == ECHILD ) {
		return -1;
	} else {
		perror("waitpid error");
		return -1;
	}

}

/**
 * Sprawdzenie stanu procesów bez blokowania.
 */
void update_status(void)
{
	int status;
	pid_t pid;

	do {
		pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG);
	} while (!mark_process_status (pid, status));
}

/**
 * Informuje o zakończonych pracach i usuwa je z listy aktywnych.
 */
void do_job_notification (void)
{
	job *j, *jlast, *jnext;

	/* Update status information for child processes.  */
	update_status ();

	jlast = NULL;
	for (j = first_job; j; j = jnext) {
		jnext = j->next;

		/* If all processes have completed, tell the user the job has
		completed and delete it from the list of active jobs.  */
		if (job_is_completed (j)) {
			format_job_info (j, "completed");
			if (jlast) {
				jlast->next = jnext;
			} else {
				first_job = jnext;
			}

			free_job (j);
		} else if (job_is_stopped (j) && !j->notified) {
			format_job_info (j, "stopped");
			j->notified = 1;
			jlast = j;
		} else
			jlast = j;
	}
}

/**
 * Pobierz informacje o procesach które mają informacje do przekazania.
 * Funkcja blokuje do czasu zebrania informacji o wszystkich procesach
 * dotyczących konkretnego job'a.
 */
void wait_for_job( job * j )
{
	int status;
	pid_t pid;

	do {
		pid = waitpid(WAIT_ANY, &status, WUNTRACED);
	} while( !mark_process_status(pid, status) && !job_is_stopped(j) && !job_is_completed(j) );
}

/**
 * Przenieś prace j do pierwszego planu. Jeśli cont jest różne od zera
 * to przywróć ustawienia terminala i wyślij wszystkim procesom pracy j
 * sygnał kontynuowania (SIGCONT).
 */
void put_job_in_foreground( job * j, int cont )
{
	j->foreground = 1;

	// ustaw prace jako pierwszoplanową
	tcsetpgrp( shell_terminal, j->pgid );

	// wyślij wszystkim procesom w pracy sygnał kontynuowania
	if( cont ) {
		tcsetattr( shell_terminal, TCSADRAIN, &j->tmodes);
		if( kill(- j->pgid, SIGCONT) < 0 ) {
			perror("kill(SIGCONT) error");
		}
	}

	// Czekaj aż procesy w pracy się zakończą
	wait_for_job(j);

	// Przenieś shell z powrotem do pierwszego planu
	tcsetpgrp( shell_terminal, shell_pgid );

	// Przywróć ustawienia terminala takie jak były dla shella
	tcgetattr( shell_terminal, &j->tmodes );
	tcsetattr( shell_terminal, TCSADRAIN, &shell_tmodes );
}

/**
 * Przenieś procesy pracy j do drugoplanowego wykonania.
 * Jeśli cont jest niezerowy wyślij procesom sygnał kontynuowania.
 */
void put_job_int_background( job *j, int cont )
{
	j->foreground = 0;

	if( cont ) {
		if( kill( - j->pgid, SIGCONT ) < 0 ) {
			perror("kill(SIGCONT) error");
		}
	}
}

/**
 * Uruchom proces
 */
void launch_process( process *p, pid_t pgid, int infile, int outfile, int errfile, int foreground)
{
	pid_t pid;

	if( is_shell_interactive ) {

		pid = getpid();
		if( pgid == 0 ) {
			pgid = pid;
		}

		setpgid(pid, pgid);

		if( foreground ) {
			tcsetpgrp( shell_terminal, pgid );
		}

		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		signal(SIGTTIN, SIG_DFL);
		signal(SIGTTOU, SIG_DFL);
		signal(SIGCHLD, SIG_DFL);
	}

	if( infile != STDIN_FILENO ) {
		dup2(infile, STDIN_FILENO);
		close(infile);
	}

	if( outfile != STDOUT_FILENO ) {
		dup2(outfile, STDOUT_FILENO);
		close(outfile);
	}

	if(errfile != STDERR_FILENO ) {
		dup2(errfile, STDERR_FILENO);
		close(errfile);
	}

	execvp(p->argv[0], p->argv);
	perror("execvp error");
	exit(1);
}

/**
 * Uruchom joba
 */
void launch_job( job *j, int foreground )
{
	process * p;
	pid_t pid;
	int pipeIO[2], infile, outfile;
	j->foreground = foreground;

	infile = j->stdin;
	for( p = j->first_process ; p ; p=p->next ){

		//jeśli więcej niż jeden proces to używamy pipe
		if(p->next) {
			if( pipe(pipeIO) < 0  ) {
				perror("pipe error");
				exit(1);
			}
			outfile = pipeIO[1];
		} else {
			outfile = j->stdout;
		}

		pid = fork();

		if( pid == 0 ) {
			// dziecko
			launch_process(p, j->pgid, infile, outfile, j->stderr, foreground);
		} else if ( pid < 0 ) {
			// niepowodzenie forka
			perror("fork error");
			exit(1);
		} else {
			// proces macierzysty

			p->pid = pid;
			if( is_shell_interactive ) {

				// ustawienie grupy procesów dla tego joba.
				// pgid jest rowne pid pierwszego procesu w jobie
				if( !j->pgid ) {
					j->pgid = pid;
				}

				setpgid(pid, j->pgid);
			}
		}

		// zamknięcie niepotrzebnych deskryptorów
		if(infile != j->stdin ) {
			close(infile);
		}

		if(outfile != j->stdout ) {
			close(outfile);
		}

		infile = pipeIO[0];
	}

	format_job_info( j , "launched");

	if( !is_shell_interactive ) {
		wait_for_job(j);
	} else if( foreground ) {
		put_job_in_foreground(j, 0);
	} else {
		put_job_int_background(j , 0);
	}
}

job * create_job() {
	job *actual_job = first_job;
	job * new_job = (job*) malloc(sizeof(job));
	new_job->next = NULL;
	new_job->first_process = NULL;
	new_job->stdin = STDIN_FILENO;
	new_job->stdout = STDOUT_FILENO;
	new_job->stderr = STDERR_FILENO;

	if(first_job == NULL) {
		first_job = new_job;
	} else {
		while(actual_job->next != NULL)
			actual_job = actual_job->next;
		actual_job->next = new_job;
	}
	return new_job;
}

process * create_process_in_last_job() {
	if(first_job == NULL) 
		return NULL;
	process * p = (process*)malloc(sizeof(process));
	p->next = NULL;
	p->argv = NULL;
	p->pid = 0;
	p->completed = 0;
	p->stopped = 0;
	p->status = 0;
	job * actual_job = first_job;
	while(actual_job->next != NULL)
		actual_job = actual_job->next;
	
	process * last_process = actual_job->first_process;
	if(last_process == NULL) {
		actual_job->first_process = p;
	} else {
		while(last_process->next != NULL) {
			last_process = last_process->next;
		}
		last_process->next = p;
	}
	return p;
}

process * get_last_process() {
	if(first_job == NULL) 
		return NULL;
	job * actual_job = first_job;
	while(actual_job->next != NULL)
		actual_job = actual_job->next;
	process * last_process = actual_job->first_process;
	if(last_process == NULL) {
		return NULL;
	} else {
		while(last_process->next != NULL) {
			last_process = last_process->next;
		}
	}
	return last_process;
}
