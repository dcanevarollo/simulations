#include "queue.h"
#include "memory.h"

memory *memStart = NULL, *memEnd = NULL;
list *swapStart = NULL, *swapEnd = NULL;

void initializeMemory() {
    memory *mem = (memory *) malloc(sizeof(memory));

    mem->proc = NULL;
    mem->size = 128;  // Max memory size.
    mem->next = NULL;
    mem->prev = NULL;
    memStart = mem;
    memEnd = mem;
}

void storeProcess(process *proc) {
    memory *cell = NULL, *temp = NULL, *bestFit = NULL;
    int dif = 0, smaller = 128;

    /* Scrolls through the memory to find the smallest fragment
     * possible for the insertion of the process. */
    temp = memStart;
    while (temp != NULL) {
        if (temp->proc == NULL && temp->size >= proc->size) {
            dif = (temp->size - proc->size);
            if (dif < smaller) {
                smaller = dif;
                bestFit = temp;  // Save the fragment with potential to be the best fit.
            }
        }

        temp = temp->next;
    }

    if (bestFit != NULL) {
        /* Fragments the memory if there is space left.*/
        if (bestFit->size > proc->size) {
            cell = (memory *) malloc(sizeof(memory));  // Free memory cell.

            cell->proc = NULL;
            cell->size = (bestFit->size - proc->size);
            cell->next = bestFit->next;
            cell->prev = bestFit;

            if (cell->next == NULL)
                memEnd = cell;
            else
                cell->next->prev = cell;

            bestFit->next = cell;
            bestFit->size = proc->size;
        }
        bestFit->proc = proc;

        changeState(proc, ready);
        insertIntoQueue(proc);

        printf("Process %d (%dw) inserted in memory; ready to execution.\n", proc->id, proc->size);
    } else if (getProcessInSwapNumber() <= 3) {  // If it does not have space for the process it is inserted into the swap.
        printf("Insufficient space in memory; process %d moved to swap.\n", proc->id);
        changeState(proc, swap);
        moveToSwap(proc);
    } else {
        printf("Both memory and swap has no available space; process %d cannot be created.\n", proc->id);
        free(proc);
    }
}

void moveToSwap(process *proc) {
    list *newSwap = (list *) malloc(sizeof(list)), *temp = NULL, *prev = NULL;

    newSwap->proc = proc;
    if (swapStart == NULL) {
        newSwap->prox = NULL;
        swapStart = newSwap;
        swapEnd = newSwap;
    } else {
        temp = swapStart;
        while (temp != NULL && temp->proc->size < proc->size) {
            prev = temp;
            temp = temp->prox;
        }

        if (temp == swapStart) {
            newSwap->prox = temp;
            swapStart = newSwap;
        } else if (temp == NULL) {
            newSwap->prox = NULL;
            swapEnd->prox = newSwap;
            swapEnd = newSwap;
        } else {
            prev->prox = newSwap;
            newSwap->prox = temp;
        }
    }
}

void removeProcess(int processId) {
    memory *cell = NULL, *excess = NULL;
    bool processRemoved = false;

    cell = memStart;
    while (cell != NULL) {
        if (cell->proc != NULL && cell->proc->id == processId) {
            cell->proc = NULL;
            printf("Process %d removed from memory.\n", processId);
            processRemoved = true;
            break;
        }

        cell = cell->next;
    }

    /* Defragments the memory. */
    if (processRemoved) {
        if (cell->next != NULL && cell->next->proc == NULL) {
            excess = cell->next;
            cell->size += excess->size;

            if (excess->next != NULL)
                excess->next->prev = cell;

            cell->next = excess->next;

            if (cell->next == NULL)
                memEnd = cell;

            free(excess);
        }

        /* Eliminates the excess predecessor to the 'cell' fragment. */
        if (cell->prev != NULL && cell->prev->proc == NULL) {
            excess = cell->prev;
            cell->size += excess->size;

            if (excess->prev != NULL)
                excess->prev->next = cell;

            cell->prev = excess->prev;

            if (cell->prev == NULL)
                memStart = cell;

            free(excess);
        }

        if (swapStart != NULL) {
            list *tempSwap = NULL;
            process *proc = NULL;

            proc = swapStart->proc;
            printf("Checking the possibility of process %d entering main memory...\n", proc->id);
            tempSwap = swapStart;
            swapStart = swapStart->prox;
            free(tempSwap);
            storeProcess(proc);
        }
    }
}

void showMemory() {
    list *tempSwap = NULL;
    memory *cell = NULL;

    printf("\n################################################# MAIN MEMORY ##########################################################\n");

    if (swapStart != NULL) {
        printf("Swap:\t\t|\t");
        tempSwap = swapStart;
        while (tempSwap != NULL) {
            printf("%d (%dw) ", tempSwap->proc->id, tempSwap->proc->size);
            tempSwap = tempSwap->prox;
        }
        printf("\n------------------------------------------------------------------------------------------------------------------------\n");
    }

    printf("\nAvailable space: %dw\n", getAvailableSpace());

    printf("Allocated processes: ");
    if (getAvailableSpace() < 128) {
        cell = memStart;
        while (cell != NULL) {
            if (cell->proc != NULL)
                printf("%d (%dw) ", cell->proc->id, cell->size);

            cell = cell->next;
        }
    } else
        printf("None.");

    printf("\n\nNULL <--- ");
    cell = memStart;
    while (cell != NULL) {
        if (cell->proc == NULL)
            printf("|F %dw|", cell->size);
        else
            printf("|P %dw|", cell->size);

        if (cell->next != NULL)
            printf(" <---> ");
        else
            printf(" ---> ");

        cell = cell->next;
    }
    printf("NULL\n");
}

bool hasProcesses() {
    if (getAvailableSpace() < 128 || swapStart != NULL)
        return true;

    return false;
}

int getProcessInSwapNumber() {
    int count = 0;

    list *temp = swapStart;
    while (temp != NULL) {
        count++;
        temp = temp->prox;
    }

    return count;
}

int getAvailableSpace() {
    memory *cell = NULL;
    int count = 0;

    cell = memStart;
    while (cell != NULL) {
        if (cell->proc == NULL)
            count += cell->size;

        cell = cell->next;
    }

    return count;
}