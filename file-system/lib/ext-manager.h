#ifndef EXT_MANAGER_H
#define EXT_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "file-manager.h"

#ifdef WIN32
    #define OS 'W'
    #define clearScreen system("cls")
    #define clearBuffer fflush(stdin);
#else
    #define OS 'U'
    #define clearScreen system("clear")
    #define clearBuffer __fpurge(stdin)

    #include <stdio_ext.h>
#endif

int readData();

void writeData();

void createExternalFile(file *f);

void openExternalFile(iNode *noI);

#endif