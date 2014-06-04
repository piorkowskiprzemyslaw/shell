#include <util/initialization.h>

char * userName;
char * mashineName;
char * actualDir;
size_t maxUserInputSize;
char * inputString;
pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int is_shell_interactive;
job * first_job;
std::map<std::string,std::string> variables;

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
		//signal(SIGCHLD, SIG_IGN);

		//signal(SIGCHLD, signalHandler);

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
 * aktualizuje wszystkie zmienne pokazywane przed znakiem zachęty
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
	if( strstr(fullDir, home) != NULL ){
		actualDir[0] = '~';
		strcpy(actualDir + 1, fullDir + strlen(home));
	} else {
		strcpy(actualDir, fullDir);
	}
}
