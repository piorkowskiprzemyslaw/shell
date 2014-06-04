#ifndef VARIABLES_H_
#define VARIABLES_H_

#include <sys/types.h>
#include <termios.h>
#include <map>
#include <string>

#include <util/globalVariables.h>

/**
 * Ustawia zawartość zmiennej o nazwie key na value
 */
void setVariable(const char *key,const char *value);

/**
 * Zwraca zawartość zmiennej o nazwie key, jeśli zmienna nie istnieje zwraca null pointer
 */
const char * getVariable(const char *key);

#endif /* VARIABLES_H_ */
