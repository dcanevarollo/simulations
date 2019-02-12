#include "disc-manager.h"

#ifdef WIN32
    #include "ext-manager.h"
#else
    #include "../lib/gerextunix.h"
#endif

disc *dsc = NULL;

/* Initializes the disc on the system as a block array. */
void initializeDisc() {
    int i = 0;
    block *blc = NULL;

    /* In this implementation we have 300 blocks available on disc. */
    dsc = (disc *) malloc(300 * sizeof(disc));

    for (i = 0; i < 300; i++) {
        blc = (block *) malloc(sizeof(block));

        blc->id = i;
        blc->fileId = 0;
        blc->nextId = -1;
        blc->next = NULL;

        dsc[i].blocks = blc;
    }
}

void showDisc() {
    int i = 0;

    clearScreen;
    printf("DISC REPRESENTATION\n\n");
    printf("-----------------------------------------------------------------------------------------------------------------\n");
    for (i = 0; i < 300; i++)
        printf("%d\t", dsc[i].blocks->fileId);
    printf("-----------------------------------------------------------------------------------------------------------------\n\n");

    printf("Press ENTER to return to the main menu");
    getchar();
}

void showTable() {
    int i = 0;

    clearScreen;
    printf("TABLE IN MEMORY REPRESENTATION\n\n");
    printf("\t ---------------\n");
    for (i = 0; i < 300; i++) {
        if (dsc[i].blocks->fileId != 0)
            printf("%d\t|\t%d\t|\n", i, dsc[i].blocks->nextId);
        else
            printf("%d\t|\t%s\t|\n", i, " ");
    }
    printf("\t ---------------\n\n");

    printf("Press ENTER to return to the main menu");
    getchar();
}

/* Returns the available space in disc. */
int getAvailableSpace() {
    int i = 0, availableSpace = 0;

    for (i = 0; i < 300; i++)
        if (dsc[i].blocks->fileId == 0)
            availableSpace++;

    return availableSpace;
}

/* Returns the first available block in disc. */
block *getAvailableBlock() {
    int i = 0;

    for (i = 0; i < 300; i++) {
        if (dsc[i].blocks->fileId == 0)
            return dsc[i].blocks;
    }

    return NULL;
}
