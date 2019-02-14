#include "queue.h"
#include "process-manager.h"

pcb *createPcb(int processId, int processQuantum) {
    pcb *newPcb = (pcb *) malloc(sizeof(pcb));

    newPcb->processId = processId;
    newPcb->processState = swap;
    newPcb->processQuantum = processQuantum;

    return newPcb;
}

process *createProcess(int id, int size, int processQuantum) {
    process *newProcess = (process *) malloc(sizeof(process));

    newProcess->id = id;
    newProcess->size = size;
    newProcess->token = createPcb(id, processQuantum);

    return newProcess;
}

process *copyProcess(process *proc) {
    process *copy = (process *) malloc(sizeof(process));

    copy->id = proc->id;
    copy->token = proc->token;

    return copy;
}

process *changeState(process *proc, int newState) {
    pcb *pcb_proc = proc->token;
    pcb_proc->processState = newState;
    proc->token = pcb_proc;

    return proc;
}

/* Decrements the value of the cpu quantum of the remaining process time. */
bool execute(process *proc, int remaining) {
    int signal;
    pcb *pcb_proc = proc->token;

    while (remaining > 0 && pcb_proc->processQuantum > 0) {
        signal = getRandom(0, 100);
        if (signal <= lockedStateChance) {
            proc = changeState(proc, locked);
            printf("Process %d locked; Remaining processing time: %d.\n", pcb_proc->processId, pcb_proc->processQuantum);
            return true;
        }

        pcb_proc->processQuantum--;
        remaining--;
    }

    proc->token = pcb_proc;

    return false;
}

bool isFinished(process *proc) {
    pcb *pcb = proc->token;

    if (pcb->processQuantum <= 0)
        return true;

    return false;
}

void roundRobin() {
    queue *currentQueue = NULL;
    cpu *central = NULL;
    process *newProcess = NULL;
    int signal = 0, processCount = 3, stage = 1;
    bool isLocked = false;

    /* Performs escalation while there are processes in the queue. */
    while (hasProcesses()) {
        if (stage > 1)
            clearScreen;
        printf("\n##################################################### RODADA %d ########################################################\n\n", stage);

        currentQueue = getQueue(ready);
        /* Executes the 1st process of the queue of ready if it is not empty. */
        if (currentQueue != NULL) {
            central = decode(currentQueue->proc);  // Inserts the process in the cpu to its processing.

            removeFromQueue(central->proc);

            central->proc = changeState(central->proc, executing);
            printf("Executing process %d...\n", central->proc->id);
            isLocked = execute(central->proc, cpuQuantum);

            /* Process goes to the finished queue when your processing time has run out. */
            if (isFinished(central->proc)) {
                changeState(central->proc, finished);
                printf("Process %d finished\n", central->proc->id);

                removeProcess(central->proc->id);
            } else if (!isLocked) {
                pcb *processToken = central->proc->token;
                changeState(central->proc, ready);

                printf("Process %d returned to the ready queue.\n", central->proc->id);
                printf("Remaining processing time to the process %d: %dms.\n", processToken->processId, processToken->processQuantum);
            }

            /* Inserts in the queue pertinent to its current state (ready, locked or finished) */
            insertIntoQueue(central->proc);
            central->proc = NULL;
        }

        currentQueue = getQueue(locked);
        if (currentQueue != NULL) {  // Tries to unlock the locked processes.
            signal = getRandom(0, 100);

            printf("Process %d waiting for device input/output.\n", currentQueue->proc->id);
            if (signal <= unlockChance) {
                printf("Process %d unlocked!\n", currentQueue->proc->id);
                exchangeQueues(locked, ready);
            }
        }

        if (stage <= 25)
            signal = getRandom(0, 100);
        else
            signal = getRandom(30, 100);

        /* Tries to create a new process. */
        if (signal <= newStateChance) {
            newProcess = createProcess(++processCount, getRandom(8, 64), getRandom(1, 100));
            storeProcess(newProcess);
        }

        showMemory();
        showTable();
        sleep(5);
        stage++;
    }
}