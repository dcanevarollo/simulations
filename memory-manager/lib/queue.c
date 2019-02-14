#include "queue.h"

queue *readyStart = NULL, *readyEnd = NULL, *deviceStart = NULL,
    *deviceEnd = NULL, *finishedStart = NULL, *finishedEnd = NULL;

void insertIntoQueue(process *proc) {
    queue *newItem = (queue *) malloc(sizeof(queue));
    pcb *processToken = proc->token;

    newItem->proc = proc;
    if (processToken->processState == ready) {
        if (readyStart == NULL) {
            readyStart = newItem;
            readyEnd = newItem;
        } else {
            readyEnd->next = newItem;
            readyEnd = newItem;
        }
    } else if (processToken->processState == locked) {
        if (deviceStart == NULL) {
            deviceStart = newItem;
            deviceEnd = newItem;
        } else {
            deviceEnd->next = newItem;
            deviceEnd = newItem;
        }
    } else if (processToken->processState == finished) {
        if (finishedStart == NULL) {
            finishedStart = newItem;
            finishedEnd = newItem;
        } else {
            finishedEnd->next = newItem;
            finishedEnd = newItem;
        }
    }

    newItem->next = NULL;
}

void removeFromQueue(process *proc) {
    queue *temp = NULL;
    pcb *processTemp = proc->token;

    if (processTemp->processState == ready) {
        temp = readyStart;
        readyStart = readyStart->next;
    } else if (processTemp->processState == locked) {
        temp = deviceStart;
        deviceStart = deviceStart->next;
    } else if (processTemp->processState == finished){
        temp = finishedStart;
        finishedStart = finishedStart->next;
    }

    free(temp);
}

void exchangeQueues(int state1, int state2) {
    queue *temp = NULL;
    process *copy = NULL;

    if (state1 == ready)
        temp = readyStart;
    else if (state1 == locked)
        temp = deviceStart;
    else if (state1 == finished)
        temp = finishedStart;

    if (temp != NULL) {
        copy = copyProcess(temp->proc);
        removeFromQueue(temp->proc);
        copy = changeState(copy, state2);

        insertIntoQueue(copy);
    }
}

void showTable() {
    queue *tempReady, *tempDevice, *tempFinished;

    tempReady = readyStart;
    tempDevice = deviceStart;
    tempFinished = finishedStart;

    printf("\n##################################################### QUEUES ###########################################################\n");

    printf("Ready\t\t|\t");
    while (tempReady != NULL) {
        printf("%d  ", tempReady->proc->id);
        tempReady = tempReady->next;
    }
    printf("\n------------------------------------------------------------------------------------------------------------------------\n");

    printf("Device\t|\t");
    while (tempDevice != NULL) {
        printf("%d  ", tempDevice->proc->id);
        tempDevice = tempDevice->next;
    }
    printf("\n------------------------------------------------------------------------------------------------------------------------\n");

    printf("Finished\t|\t");
    while (tempFinished != NULL) {
        printf("%d  ", tempFinished->proc->id);
        tempFinished = tempFinished->next;
    }
    printf("\n########################################################################################################################\n\n\n");
}

queue *getQueue(int processState) {
    if (processState == ready)
        return readyStart;
    else if (processState == locked)
        return deviceStart;
    else if (processState == finished)
        return finishedStart;
}