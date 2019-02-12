#ifndef DISC_MANAGER_H
#define DISC_MANAGER_H

typedef struct _block {
    int id;
    int fileId;
    int nextId;
    struct _block *next;
} block;

typedef struct _disc {
    block *blocks;  // The disc have a array of blocks.
} disc;

void initializeDisc();

void showDisc();

void showTable();

int getAvailableSpace();

block *getAvailableBlock();

#endif