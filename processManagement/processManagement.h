/*
 * processManagement.h
 *
 *  Created on: 23 maj 2014
 *      Author: przemek
 */

#ifndef PROCESSMANAGEMENT_H_
#define PROCESSMANAGEMENT_H_

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <sys/wait.h>
#include <map>
#include <string>

#include <util/globalVariables.h>

/**
 * Znajdź pracę o zadanym pgid
 */
job * find_job(pid_t pgid);

/**
 * Sprawdza czy wszystkie procesy w ramach pracy się zakończyły
 * lub czy są zastopowane.
 */
int job_is_stopped(job * j);

/**
 * Sprawdza czy wszystkie procesy w ramach pracy zostały zakończone.
 */
int job_is_completed(job * j);

/**
 * Wyświetl wiadomość dotyczącą pracy.
 */
void format_job_info( job *j, const char *status );

/**
 * Zwalnia zasoby zajmowane przez konkretną pracę.
 */
void free_job( job * j );

/**
 * Dla pid-u który został zwrócony przez waitpid sprawdź jego status
 * i przygotuj informacje dla użytkownika o szczegółach zamknięcia tego
 * pliku.
 */
int mark_process_status(pid_t pid, int status);

/**
 * Sprawdzenie stanu procesów bez blokowania.
 */
void update_status(void);

/**
 * Informuje o zakończonych pracach i usuwa je z listy aktywnych.
 */
void do_job_notification (void);

/**
 * Pobierz informacje o procesach które mają informacje do przekazania.
 * Funkcja blokuje do czasu zebrania informacji o wszystkich procesach
 * dotyczących konkretnego job'a.
 */
void wait_for_job( job * j );

/**
 * Przenieś prace j do pierwszego planu. Jeśli cont jest różne od zera
 * to przywróć ustawienia terminala i wyślij wszystkim procesom pracy j
 * sygnał kontynuowania (SIGCONT).
 */
void put_job_in_foreground( job * j, int cont );

/**
 * Przenieś procesy pracy j do drugoplanowego wykonania.
 * Jeśli cont jest niezerowy wyślij procesom sygnał kontynuowania.
 */
void put_job_int_background( job *j, int cont );

/**
 * Uruchom proces
 */
void launch_process( process *p, pid_t pgid, int infile, int outfile, int errfile, int foreground);

/**
 * Uruchom joba
 */
void launch_job( job *j, int foreground );

/** 
 * Tworzy job i umieszcza go na końcu listy
 */
 
job * create_job();

/**
 * Tworzy proces i umieszcza go na koncu listy procesow ostatniego joba
 */
process * create_process_in_last_job();

process * get_last_process();


#endif /* PROCESSMANAGEMENT_H_ */
