#ifndef CONTROLLER_H
#define CONTROLLER_H

#ifdef WIN32
#define clearScreen system("cls")
#define clearBuffer fflush(stdin)
#else
#define clearScreen system("clear")
#define clearBuffer __fpurge(stdin)
#endif

/* Module functions as an intermediary in communications between the database access module
 * and/or control module of the B+ tree; can be interpreted as the controller of main. */

#include <stdio.h>
#include <stdlib.h>
#include "models.h"
#include "dam.h"
#include "bpt.h"

void invalid();

void abstract(page *tree);

void showStudents();

int menu();

page *registerStudent(page *tree);

page *removeStudent(page *tree);

#endif
