#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include "process-manager.h"
#include "models.h"

void insertIntoQueue(process *proc);

void removeFromQueue(process *proc);

void exchangeQueues(int state1, int state2);

void showTable();

queue *getQueue(int processState);

#endif