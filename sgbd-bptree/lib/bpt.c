#include "bpt.h"
#include "dam.h"

/*  bpt:  B+ Tree Implementation
 *
 *  Copyright (c) 2018  Amittai Aviram  http://www.amittai.com
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 *  3. The name of the copyright holder may not be used to endorse
 *  or promote products derived from this software without specific
 *  prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  Author:  Amittai Aviram
 *           http://www.amittai.com
 *           amittai.aviram@gmail.com or afa13@columbia.edu
 *  Original date: 26 June 2010
 *  Last modified: 02 September 2018 */

/* ====================================== GLOBAL VARIABLES ====================================== */

int order = 5;  // B+ tree order.

page *queue = NULL;  // Pointer used to scroll through the list of leaves.


/* ======================================= TREE STRUCTURES ====================================== */

record *createRecord(int rId) {
    record *newRecord = (record *) malloc(sizeof(record));

    newRecord->rId = rId;

    return newRecord;
}

page *createPage() {
    page *newPage = malloc(sizeof(page));

    newPage->keys = malloc(order-1 * sizeof(int));
    newPage->pointers = malloc(order * sizeof(void *));
    newPage->isLeave = false;
    newPage->keyNumber = 0;
    newPage->father = NULL;
    newPage->next = NULL;

    return newPage;
}

page *createLeave() {
    page *newLeave = createPage();

    newLeave->isLeave = true;

    return newLeave;
}

page *createNewTree(int key, record *rec) {
    page *root = createLeave();

    root->keys[0] = key;
    root->pointers[0] = rec;  // Record stored in the leave.
    root->pointers[order-1] = NULL;
    root->father = NULL;
    root->keyNumber++;

    return root;
}


/* ===================================== FUNCTION PROTOTYPES ===================================== */
page *insertIntoFather(page *root, page *leftChild, page *rightChild, int key);

page *purgeRecord(page *root, page *target, int key, void *pointer);


/* =================================== HELP AND QUERY FUNCTIONS ================================== */


/* Inserts the 'newPage' in the leaves list;
 * this method is called in order to chain the leaves. */
void enqueue(page *queuePage) {
    page *temp = NULL;

    if (queue == NULL) {
        queue = queuePage;
        queue->next = NULL;
    } else {
        temp = queue;
        while (temp->next != NULL)
            temp = temp->next;

        temp->next = queuePage;
        queuePage->next = NULL;
    }
}

/* Returns the first page of the leaves list; pages are removed
 * from the list as far as where they are returned. */
page *dequeue() {
    page *temp = queue;

    queue = queue->next;
    temp->next = NULL;

    return temp;
}

/* Returns the depth/height of the page. */
int pageHeight(page *root, page *child) {
    int size = 0;
    page *temp = NULL;

    temp = child;
    while (temp != root) {
        temp = temp->father;
        size++;
    }

    return size;
}

void showTree(page *root) {
    page *temp = NULL;
    record *rec = NULL;
    int i = 0, height = 0, newHeight = 0, totalSpace = 0, pageCount = 0;

    if (root == NULL) {
        printf("\nEmpty tree.\n");

        return;
    }

    printf("\nABSTRACT\nRecords registered\nLeaves - id:rId | Roots - id\n\n");
    queue = NULL;
    enqueue(root);
    while (queue != NULL) {
        pageCount++;
        temp = dequeue();

        if (temp->father != NULL && temp == temp->father->pointers[0]) {
            newHeight = pageHeight(root, temp);

            if (newHeight != height) {
                height = newHeight;
                printf("\n");
            }
        }

        /* Displays the key (id) and, if the page is a leave
         * also displays the rId contained in your pointer. */
        for (i = 0; i < temp->keyNumber; i++) {
            /* Increments the total space variable to return the KB stored. */
            totalSpace += (temp->keyNumber+1)*sizeof(void *) + temp->keyNumber*sizeof(int);
            printf("%d", temp->keys[i]);

            if (temp->isLeave) {
                rec = (record *) temp->pointers[i];

                printf(":%d ", rec->rId);
            }
        }

        /* If the page is not leave your child pages are inserted in the list of pages to be displayed. */
        if (!temp->isLeave)
            for (i = 0; i <= temp->keyNumber; i++)
                enqueue(temp->pointers[i]);

        printf("| ");
    }

    printf("\n\n%d pages.\n%d KB.\n\n", pageCount, totalSpace);
}

/* Function that does the page cut; returns the middle position to the page division. */
int cut(int size) {
    if (size%2 == 0)
        return size/2;
    else
        return size/2 + 1;
}

/* Searchs an id on the leave and returns the corresponding record. */
page *findLeave(page *root, int key, int op) {
    int i = 0;
    page *cursor = NULL;

    if (root == NULL) {
        if (op == search)
            printf("\nEmpty tree.\n");

        return root;
    }

    cursor = root;
    while (!cursor->isLeave) {
        i = 0;

        while (i < cursor->keyNumber){
            if (key >= cursor->keys[i])
                i++;
            else
                break;
        }

        cursor = (page *) cursor->pointers[i];
    }

    return cursor;
}

record *findRecord(page *root, int key, int op) {
    int i = 0;
    page *leave = NULL;

    if (root == NULL) {
        if (op == search)
            printf("\nEmpty tree.\n");

        return NULL;
    }

    /* Get the leave that the record is contained. */
    leave = findLeave(root, key, op);

    /* Searchs the corresponding key. */
    for (i = 0; i < leave->keyNumber; i++)
        if (leave->keys[i] == key)
            break;

    if (i == leave->keyNumber)
        return NULL;
    else
        return (record *) leave->pointers[i];
}


/* ==================================== INSERTION FUNCTIONS ====================================== */

/* Returns the index at which the parent page should point.
 * Later, the cut is made on the left page of the root. */
int getLeftIndex(page *father, page *leftChild) {
    int leftIndex = 0;

    while (leftIndex <= father->keyNumber && father->pointers[leftIndex] != leftChild)
        leftIndex++;

    return leftIndex;
}

page *insertIntoRoot(page *root, page *father, page *leftChild, int leftIndex, int key) {
    int i = 0;

    for (i = father->keyNumber; i > leftIndex; i--) {
        father->pointers[i+1] = father->pointers[i];
        father->keys[i] = father->keys[i-1];
    }

    father->pointers[leftIndex+1] = leftChild;
    father->keys[leftIndex] = key;
    father->keyNumber++;

    return root;
}

page *insertIntoNewRoot(page *leftChild, page *rightChild, int key) {
    page *newRoot = createPage();

    newRoot->keys[0] = key;
    newRoot->pointers[0] = leftChild;
    newRoot->pointers[1] = rightChild;
    newRoot->keyNumber++;
    newRoot->father = NULL;

    leftChild->father = newRoot;
    rightChild->father = newRoot;

    return newRoot;
}

page *insertIntoFatherAfterSplitting(page *root, page *oldPage, page *rightChild, int leftIndex, int key) {
    page *newPage = NULL, *child = NULL, **tempPointers = NULL;
    int i = 0, j = 0, split = 0, mainKey = 0, *tempKeys = NULL;

    tempPointers = malloc(order+1 * sizeof(page *));
    tempKeys = malloc(order * sizeof(int));

    for (i = 0, j = 0; i < oldPage->keyNumber+1; i++, j++) {
        if (j == leftIndex+1)
            j++;

        tempPointers[j] = oldPage->pointers[i];
    }

    for (i = 0, j = 0; i < oldPage->keyNumber; i++, j++) {
        if (j == leftIndex)
            j++;

        tempKeys[j] = oldPage->keys[i];
    }

    tempPointers[leftIndex+1] = rightChild;
    tempKeys[leftIndex] = key;

    split = cut(order);
    newPage = createPage();
    oldPage->keyNumber = 0;

    for (i = 0; i < split-1; i++) {
        oldPage->pointers[i] = tempPointers[i];
        oldPage->keys[i] = tempKeys[i];
        oldPage->keyNumber++;
    }

    oldPage->pointers[i] = tempPointers[i];
    mainKey = tempKeys[split-1];

    for (++i, j = 0; i < order; i++, j++) {
        newPage->pointers[j] = tempPointers[i];
        newPage->keys[j] = tempKeys[i];
        newPage->keyNumber++;
    }

    newPage->pointers[j] = tempPointers[i];
    newPage->father = oldPage->father;

    free(tempPointers);
    free(tempKeys);

    for (i = 0; i <= newPage->keyNumber; i++) {
        child = newPage->pointers[i];
        child->father = newPage;
    }

    return insertIntoFather(root, oldPage, newPage, mainKey);
}

page *insertIntoFather(page *root, page *leftChild, page *rightChild, int key) {
    page *father = NULL;
    int leftIndex = 0;

    father = leftChild->father;

    if (father == NULL)
        return insertIntoNewRoot(leftChild, rightChild, key);

    leftIndex = getLeftIndex(father, leftChild);

    if (father->keyNumber < order-1)
        return insertIntoRoot(root, father, rightChild, leftIndex, key);

    return insertIntoFatherAfterSplitting(root, father, rightChild, leftIndex, key);
}

page *insertIntoLeaveAfterSplitting(page *root, page *leave, int key, record *rec) {
    page *newLeave = NULL;
    int *tempKeys = NULL;
    void **tempPointers = NULL;
    int i = 0, j = 0, leftIndex = 0, split = 0, copyId = 0;

    newLeave = createLeave();
    tempKeys = malloc(order * sizeof(int));
    tempPointers = malloc(order * sizeof(void *));

    while (leftIndex < order-1 && leave->keys[leftIndex] < key)
        leftIndex++;

    for (i = 0, j = 0; i < leave->keyNumber; i++, j++) {
        if (j == leftIndex)
            j++;

        tempKeys[j] = leave->keys[i];
        tempPointers[j] = leave->pointers[i];
    }

    tempKeys[leftIndex] = key;
    tempPointers[leftIndex] = rec;

    leave->keyNumber = 0;
    split = cut(order - 1);

    for (i = 0; i < split; i++) {
        leave->pointers[i] = tempPointers[i];
        leave->keys[i] = tempKeys[i];
        leave->keyNumber++;
    }

    for (i = split, j = 0; i < order; i++, j++) {
        newLeave->pointers[j] = tempPointers[i];
        newLeave->keys[j] = tempKeys[i];
        newLeave->keyNumber++;
    }

    free(tempPointers);
    free(tempKeys);

    newLeave->pointers[order-1] = leave->pointers[order-1];
    leave->pointers[order-1] = newLeave;

    for (i = leave->keyNumber; i < order-1; i++)
        leave->pointers[i] = NULL;

    for (i = newLeave->keyNumber; i < order-1; i++)
        newLeave->pointers[i] = NULL;

    newLeave->father = leave->father;
    copyId = newLeave->keys[0];

    return insertIntoFather(root, leave, newLeave, copyId);
}

page *insertIntoLeave(page *leave, int key, record *rec) {
    int i = 0, leftIndex = 0;

    while (leftIndex < leave->keyNumber && leave->keys[leftIndex] < key)
        leftIndex++;

    for (i = leave->keyNumber; i > leftIndex; i--) {
        leave->keys[i] = leave->keys[i-1];
        leave->pointers[i] = leave->pointers[i-1];
    }

    leave->keys[leftIndex] = key;
    leave->pointers[leftIndex] = rec;
    leave->keyNumber++;

    return leave;
}

/* Main insertion function;
 * The 'verbose' paramater decides if a feedback message will be returned to the user. */
page *insertRecord(page *root, int rId, student *newStudent, bool verbose) {
    record *newRecord = NULL;
    page *leave = NULL;

    /* Only inserts the student record on the tree if he is actived. */
    if (newStudent->status == 'A') {
        newRecord = findRecord(root, newStudent->studentId, insertion);
        if (newRecord != NULL) {
            printf("ERROR!\nId %d already exists.\n", newStudent->studentId);

            return root;
        }

        newRecord = createRecord(rId);

        if (root == NULL)
            root = createNewTree(newStudent->studentId, newRecord);
        else {
            leave = findLeave(root, newStudent->studentId, insertion);

            if (leave->keyNumber < order - 1)
                insertIntoLeave(leave, newStudent->studentId, newRecord);
            else
                root = insertIntoLeaveAfterSplitting(root, leave, newStudent->studentId, newRecord);
        }
    }

    /* Stores the student in the database. */
    saveStudent(rId, newStudent, verbose);

    return root;
}


/* ====================================== DELETE FUNCTIONS ======================================= */

int getNeighbourIndex(page *target) {
    int i = 0;

    for (i = 0; i <= target->father->keyNumber; i++)
        if (target->father->pointers[i] == target)
            return i-1;

    printf("Search for a nonexistent pointer.\n");
    exit(EXIT_FAILURE);
}

page* redistributePages(page *root, page *target, page *neighbour,
        int neighbourIndex, int mainKeyIndex, int mainKey) {
    int i = 0;
    page *temp = NULL;

    if (neighbourIndex != -1) {
        if (!target->isLeave)
            target->pointers[target->keyNumber+1] = target->pointers[target->keyNumber];

        for (i = target->keyNumber; i > 0; i--) {
            target->keys[i] = target->keys[i-1];
            target->pointers[i] = target->pointers[i-1];
        }

        if (!target->isLeave) {
            target->pointers[0] = neighbour->pointers[neighbour->keyNumber];
            temp = (page *) target->pointers[0];
            temp->father = target;
            neighbour->pointers[neighbour->keyNumber] = NULL;
            target->keys[0] = mainKey;
            target->father->keys[mainKeyIndex] = neighbour->keys[neighbour->keyNumber-1];
        } else {
            target->pointers[0] = neighbour->pointers[neighbour->keyNumber-1];
            neighbour->pointers[neighbour->keyNumber-1] = NULL;
            target->keys[0] = neighbour->keys[neighbour->keyNumber-1];
            target->father->keys[mainKeyIndex] = target->keys[0];
        }
    } else {
        if (target->isLeave) {
            target->keys[target->keyNumber] = neighbour->keys[0];
            target->pointers[target->keyNumber] = neighbour->pointers[0];
            target->father->keys[mainKeyIndex] = neighbour->keys[1];
        } else {
            target->keys[target->keyNumber] = mainKey;
            target->pointers[target->keyNumber+1] = neighbour->pointers[0];
            temp = (page *) target->pointers[target->keyNumber+1];
            temp->father = target;
            target->father->keys[mainKeyIndex] = neighbour->keys[0];
        }

        for (i = 0; i < neighbour->keyNumber - 1; i++) {
            neighbour->keys[i] = neighbour->keys[i+1];
            neighbour->pointers[i] = neighbour->pointers[i+1];
        }

        if (!target->isLeave)
            neighbour->pointers[i] = neighbour->pointers[i+1];
    }

    target->keyNumber++;
    neighbour->keyNumber--;

    return root;
}

page *mixPages(page *root, page *target, page *neighbour, int neighbourIndex, int mainKey) {
    page *temp = NULL;
    int i = 0, j = 0, neighbourInsertionIndex = 0, targetEnd = 0;

    if (neighbourIndex == -1) {
        temp = target;
        target = neighbour;
        neighbour = temp;
    }

    neighbourInsertionIndex = neighbour->keyNumber;

    if (!target->isLeave) {
        neighbour->keys[neighbourInsertionIndex] = mainKey;
        neighbour->keyNumber++;

        targetEnd = target->keyNumber;
        for (i = neighbourInsertionIndex+1, j = 0; j < targetEnd; i++, j++) {
            neighbour->keys[i] = target->keys[j];
            neighbour->pointers[i] = target->pointers[j];
            neighbour->keyNumber++;
            target->keyNumber--;
        }

        neighbour->pointers[i] = target->pointers[j];

        for (i = 0; i < neighbour->keyNumber + 1; i++) {
            temp = (page *)neighbour->pointers[i];
            temp->father = neighbour;
        }
    } else {
        for (i = neighbourInsertionIndex, j = 0; j < target->keyNumber; i++, j++) {
            neighbour->keys[i] = target->keys[j];
            neighbour->pointers[i] = target->pointers[j];
            neighbour->keyNumber++;
        }

        neighbour->pointers[order-1] = target->pointers[order-1];
    }

    root = purgeRecord(root, target->father, mainKey, target);
    free(target->keys);
    free(target->pointers);
    free(target);

    return root;
}

page *adjustRoot(page *root) {
    page *newRoot = NULL;

    if (root->keyNumber > 0)
        return root;

    if (!root->isLeave) {
        newRoot = root->pointers[0];
        newRoot->father = NULL;
    }
    else
        newRoot = NULL;

    free(root->keys);
    free(root->pointers);
    free(root);

    return newRoot;
}

page *deleteRecordOfPage(page *target, int key, page *pointer) {
    int i = 0, pointersCount = 0;

    while (target->keys[i] != key)
        i++;

    for (++i; i < target->keyNumber; i++)
        target->keys[i-1] = target->keys[i];

    pointersCount = target->isLeave ? target->keyNumber : target->keyNumber + 1;

    i = 0;
    while (target->pointers[i] != pointer)
        i++;

    for (++i; i < pointersCount; i++)
        target->pointers[i-1] = target->pointers[i];

    target->keyNumber--;

    if (target->isLeave)
        for (i = target->keyNumber; i < order - 1; i++)
            target->pointers[i] = NULL;
    else
        for (i = target->keyNumber + 1; i < order; i++)
            target->pointers[i] = NULL;

    return target;
}

page *purgeRecord(page *root, page *target, int key, void *pointer) {
    int capacity = 0, minKeys = 0, neighbourIndex = 0, mainKeyIndex = 0, mainKey = 0;
    page *neighbour = NULL;

    target = deleteRecordOfPage(target, key, pointer);

    if (target == root)
        return adjustRoot(root);

    minKeys = target->isLeave ? cut(order - 1) : cut(order)-1;

    if (target->keyNumber >= minKeys)
        return root;

    neighbourIndex = getNeighbourIndex(target);

    mainKeyIndex = neighbourIndex == -1 ? 0 : neighbourIndex;
    mainKey = target->father->keys[mainKeyIndex];

    neighbour = neighbourIndex == -1 ? target->father->pointers[1] : target->father->pointers[neighbourIndex];

    capacity = target->isLeave ? order : order - 1;

    if (neighbour->keyNumber + target->keyNumber < capacity)
        return mixPages(root, target, neighbour, neighbourIndex, mainKey);
    else
        return redistributePages(root, target, neighbour, neighbourIndex, mainKeyIndex, mainKey);
}

/* Main delete function. */
page *deleteRecord(page *root, int id) {
    page *leaveKey = NULL;
    record *recordKey = NULL;
    int rId = 0;

    leaveKey = findLeave(root, id, insertion);
    recordKey = findRecord(root, id, search);

    if (recordKey != NULL && leaveKey != NULL) {
        rId = recordKey->rId;
        root = purgeRecord(root, leaveKey, id, recordKey);

        deleteStudent(rId);
    }


    return root;
}
