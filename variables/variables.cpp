#include <variables/variables.h>

/**
 * Ustawia zawartość zmiennej o nazwie key na value
 */
void setVariable(const char *key,const char *value)
{
	variables[std::string(key)] = std::string(value);
}

/**
 * Zwraca zawartość zmiennej o nazwie key, jeśli zmienna nie istnieje zwraca null pointer
 */
const char * getVariable(const char *key)
{
	std::map<std::string,std::string>::iterator it = variables.find(std::string(key));
	if(it != variables.end())
		return it->second.c_str();
	else
		return NULL;
}
