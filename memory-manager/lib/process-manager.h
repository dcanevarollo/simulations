#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#define cpuQuantum 40
#define newStateChance 40
#define lockedStateChance 1
#define unlockChance 50

#ifdef WIN32
    #define clearScreen system("cls")
#else
    #define clearScreen system("clear")
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "random.h"
#include "models.h"
#include "memory.h"

pcb *createPcb(int processId, int processQuantum);

process *createProcess(int id, int size, int processQuantum);

process *copyProcess(process *proc);

process *changeState(process *proc, int newState);

bool execute(process *proc, int remaining);

bool isFinished(process *proc);

void roundRobin();

#endif
