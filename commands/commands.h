#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <variables/variables.h>

/**
* Zmiana bieżącego katalogu
*/
void changeDir(const char* path);


/**
* Wczytanie wartości zmiennej o nazwie key
*/
void readVariables(char *key);

/**
* Wypisanie napisu string na standardowe wyjscie
*/
void echoString(char *string);

#endif /* COMMANDS_H_ */
