#include <stdio.h>
#include "lib/random.h"
#include "lib/models.h"
#include "lib/queue.h"
#include "lib/memory.h"
#include "lib/process-manager.h"

int main() {
    int i = 0;
    process *proc = NULL;

    initializeMemory();
    for (i = 0; i < 3; i++) {
        proc = createProcess((i + 1), getRandom(8, 64), getRandom(1, 100));
        storeProcess(proc);
    }

    roundRobin();

    return 0;
}