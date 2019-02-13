#ifndef BPT_H
#define BPT_H

/* Header of the bpt.c implementation.
 * We modified some features on the core code to our abstraction
 * and created that header to easily manipulate the access to
 * the functions that truly need to be accessed. */

#define insertion 0
#define search 1

#include <stdio.h>
#include <stdlib.h>
#include "models.h"

/* Structure that will contain the information allocated in the sheets;
 * a 'record' has a record line indicator in the file. */
typedef struct _record {
    int rId;  // The rId references the line that the tuple is contained in the database file.
} record;

/* Page that will contain the necessary records and information
 * to maintain the concept of a B+ tree. */
typedef struct _page {
    /* In the leaves the void pointers will point to the record
     * where information like rId to search in the database file will be contained. */
    void **pointers;
    int *keys;
    int keyNumber;
    bool isLeave;
    struct _page *father;
    struct _page *next;
} page;

void showTree(page *root);

record *findRecord(page *root, int key, int op);

page *insertRecord(page *root, int rId, student *newStudent, bool verbose);

page *deleteRecord(page *root, int id);

#endif
