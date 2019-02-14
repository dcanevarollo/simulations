#ifndef MODELS_H
#define MODELS_H

#define swap 1
#define ready 2
#define locked 3
#define finished 4
#define executing 5

#define bool char
#define true 1
#define false 0

#include <stdlib.h>
#include <stdio.h>

typedef struct pcb {
    int processId;
    int processState;
    int processQuantum;
} pcb;

typedef struct _process {
    int id;
    int size;
    pcb *token;
} process;

typedef struct cpu {
    process *proc;
} cpu;

typedef struct _memory {
    int size;
    process *proc;
    struct _memory *next;
    struct _memory *prev;
} memory;

typedef struct _queue {
    process *proc;
    struct _queue *next;
} queue;

typedef struct _list {
    process *proc;
    struct _list *prox;
} list;

cpu *decode(process *proc);

#endif