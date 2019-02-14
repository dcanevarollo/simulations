#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <stdio.h>
#include "models.h"

void initializeMemory();

void storeProcess(process *proc);

void moveToSwap(process *proc);

void removeProcess(int processId);

void showMemory();

bool hasProcesses();

int getProcessInSwapNumber();

int getAvailableSpace();

#endif
