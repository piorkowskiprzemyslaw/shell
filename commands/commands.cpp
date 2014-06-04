#include <commands/commands.h>

/**
* Zmiana bieżącego katalogu
*/
void changeDir(const char* path)
{
	if(chdir(path))
	{
		perror("Nie udało się zmienić bieżacego katalogu");
	}
}

/**
* Wczytanie wartości zmiennej o nazwie key
*/
void readVariables(char *key)
{
	char *line;
	size_t size=0;
	if (getline(&line, &size, stdin) == -1)
	{
		perror("Nie udało się wczytać zmiennej");
	}
	else
	{
		*strchr(line,'\n') = 0;
		setVariable(key,line);
	}
	free(line);
}

/**
* Wypisanie napisu string na standardowe wyjscie
*/
void echoString(char *string)
{
	printf("%s\n",string);
}
