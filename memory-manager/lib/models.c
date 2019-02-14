#include "models.h"

cpu *decode(process *proc) {
    cpu *newDecode = (cpu *) malloc(sizeof(cpu));

    newDecode->proc = proc;

    return newDecode;
}