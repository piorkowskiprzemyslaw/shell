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
#include <string.h>
#include <pwd.h>
#include <map>
#include <string>

#include <util/globalVariables.h>
#include <processManagement/processManagement.h>

/**
 * Inicjalizacja shella
 */
void init();

/**
 * Obsługa SIGCHLD
 */
void signalHandler(int signo);

/**
 * Zwraca aktualną ścieżke relatywną do HOME
 */
void refreshVariables();

#endif /* INITIALIZATION_H_ */
