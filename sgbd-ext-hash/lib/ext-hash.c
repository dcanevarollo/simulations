#include "ext-hash.h"

int globalDepth = 2, idCtrl = 0, dirSize = 0;
bool superOverflow = false;
directory *dir = NULL;

/* Uses the processor's clock to generate random numbers. */
int rdtsc() {
    __asm__ __volatile__("rdtsc");
}

/* Compare the equality of the binary numbers. */
int compareKeys(char *key1, char *key2, int depth) {
    int i = 0, count = 0;
    bool result = true;

    i = (strlen(key1)-1);
    while (count < depth) {
        if (key1[i] != key2[i]) {
            result = false;
            break;
        }

        count++;
        i--;
    }

    return result;
}

/* Converts a decimal number to the binary base. */
char *toBinary(int decimal, int depth) {
    int i = 0;
    char *bin = (char *) malloc(globalDepth+1 * sizeof(char));

    for (i = depth; i >= 0; i--) {
        if (i == depth) {
            bin[i] = '\0';
            continue;
        } else if (decimal%2 == 1)
            bin[i] = '1';
        else
            bin[i] = '0';

        decimal /= 2;
    }

    return bin;
}

/* Converts the binary char array to a decimal number. */
int toDecimal(char *bin) {
    int i = 0, dec = 0, indBin = 0;

    indBin = strlen(bin);
    while (indBin--)
        dec += (int) pow(2, (double) i++) * (bin[indBin]-48);  // 48 is the ASCII code of the '0' digit.

    return dec;
}

/* Hash function. */
int getHashCode(int accountNumber, int depth) {
    int index = 0;
    char *key = NULL;

    key = toBinary(accountNumber, depth);
    index = toDecimal(key);

    return index;
}

/* Finds the corresponding directory for the 'target'.
 * For example, if the target is the '100' directory then this
 * function returns a pointer to the '000' directory. */
directory *getCorrespondingDir(directory *target) {
    int i = 0, initialIndex = 0;
    directory *corresponding = NULL;

    do {
        for (i = initialIndex; i < dirSize; i++)
            if (compareKeys(target->key, dir[i].key, globalDepth-1)) {
                corresponding = &dir[i];
                break;
            }

        initialIndex = i+1;
    } while (corresponding == target);

    return corresponding;
}

void duplicateDirectories(int midIndex) {
    int i = 0;
    directory *corresponding = NULL;

    dirSize = (int) pow(2, (double) globalDepth);

    for (i = 0; i < midIndex; i++)  // Add a less significant digit in existing directories.
        dir[i].key = toBinary(i, globalDepth);

    dir = (directory *) realloc(dir, dirSize * sizeof(directory));
    for (i = midIndex; i < dirSize; i++) {
        dir[i].key = toBinary(idCtrl++, globalDepth);

        corresponding = getCorrespondingDir(&dir[i]);  // Finds the corresponding directory to remake the pointers.
        dir[i].bkt = corresponding == NULL ? NULL : corresponding->bkt;
    }
}

/* Overflow treatment: bucket division. */
void divideBucket(directory *target, client *newClient) {
    int i = 0, count = 0;
    bool temp = false;
    bucket *newBucket = NULL, *tempBucket = NULL;
    directory *corresponding = NULL;

    newBucket = createBucket(target->bkt->id+10, target->bkt->localDepth);

    /* If the keys of the new bucket are equal to those of the full bucket - considering
     * local depth -1 less significant digits - and the new bucket is not equal to the
     * full bucket, we find the new bucket directory. */
    corresponding = getCorrespondingDir(target);
    if (corresponding != NULL) {
        corresponding->bkt = newBucket;
        tempBucket = createBucket(-1, -1);  // Help bucket to store the keys of the full bucket.

        for (i = 0; i < 4; i++) {
            tempBucket->rec[i].rId = target->bkt->rec[i].rId;
            tempBucket->rec[i].key = target->bkt->rec[i].key;

            target->bkt->rec[i].rId = -1;
            target->bkt->rec[i].key = -1;
        }

        count = 0;
        for (i = 0; i < 4; i++)  // Check if the correct division of the full bucket is possible.
            if (strcmp(target->key, toBinary(tempBucket->rec[i].key, globalDepth)) == 0)
                count++;

        if (count == 4) {
            temp = true;

            /* Flag that controls whether there was persistence of overflow (indivision of the full bucket) */
            superOverflow = true;
        }

        /* If overflow persists, half of the full bucket stays and the other half goes to the new bucket. */
        if (temp) {
            for (i = 0; i < 2; i++) {
                target->bkt->rec[i].rId = tempBucket->rec[i].rId;
                target->bkt->rec[i].key = tempBucket->rec[i].key;
            }

            for (i = 2; i < 4; i++) {
                corresponding->bkt->rec[i].rId = tempBucket->rec[i].rId;
                corresponding->bkt->rec[i].key = tempBucket->rec[i].key;
            }

        } else
            for (i = 0; i < 4; i++)  // Inserts into the correct bucket each element of the bucket filled.
                insertRecord(&tempBucket->rec[i], newClient, true);

        free(tempBucket);
    }
}

int purge(directory *target, int key) {
    int i = 0, rId = 0;

    for (i = 0; i < 4; i++)
        if (target->bkt->rec[i].key == key) {
            rId = target->bkt->rec[i].rId;
            target->bkt->rec[i].rId = -1;
            target->bkt->rec[i].key = -1;

            return rId;
        }

    return -1;
}

/* Returns the rId that are referenced by the key. */
int find(directory *target, int key) {
    int i = 0;

    for (i = 0; i < 4; i++)
        if (target->bkt->rec[i].key == key)
            return target->bkt->rec[i].rId;

    return -1;
}

int getRandom(int min, int max) {
    srand(rdtsc());

    return (rand() % (max - min + 1) + min);
}

/* Main insert function. */
bool insertRecord(record *rec, client *newClient, bool hasOverflow) {
    int i = 0, index = 0;

    if (newClient->isActive) {
        index = getHashCode(rec->key, globalDepth);
        for (i = 0; i < 4; i++)  // Finds a available space in the bucket and insert the record in the same.
            if (dir[index].bkt->rec[i].key == -1) {
                dir[index].bkt->rec[i].rId = rec->rId;
                dir[index].bkt->rec[i].key = rec->key;
                break;
            }

        if (i == 4) {  // Overflow.
            /* If the global depth is equal to local depth of the full bucket
             * (increment of the full bucket location is already done). */
            if (globalDepth == dir[index].bkt->localDepth++) {
                if (globalDepth == 3) {  // Max size of global depth.
                    dir[index].bkt->localDepth--;

                    system("cls");
                    printf("ERROR!\nOverflow occurred.\nThe account %d cannot be registered.\n", newClient->accountNumber);

                    return true;
                }

                globalDepth++;
                duplicateDirectories(i);
            }

            divideBucket(&dir[index], newClient);

            /* Recursive call to insert function. Now the buckets are divided and
             * the key can be inserted on the correct index/directory. */
            insertRecord(rec, newClient, false);
        }

        if (!hasOverflow)
            storeClient(rec->rId, newClient);

        return true;
    }

    return false;
}

/* Main delete function. */
bool deleteRecord(int key) {
    int i= 0 , index = 0, rId = 0, dirtyBit = 0, pinCount = 0;
    bool hasRecords = false;
    directory *target = NULL, *corresponding = NULL;
    bucket *tempBucket = NULL;

    dirtyBit = getRandom(0, 100);
    pinCount = getRandom(0, 100);

    system("cls");
    if (dirtyBit <= dirtyBitChance) {
        printf("ERROR!\nPage under alteration.\nFailure on the removal of the record.\n");
        sleep(3);

        return false;
    } else if (pinCount <= pinCountChance) {
        printf("ERROR!\nAnother users are using this page.\nFailure on the removal of the record.\n");
        sleep(3);

        return false;
    }

    index = getHashCode(key, globalDepth);
    rId = purge(&dir[index], key);

    corresponding = getCorrespondingDir(&dir[index]);
    target = &dir[index];

    /* If you have not found the record in your supposed correct index,
     * look in the corresponding directory and do the removal. */
    if (rId == -1 && superOverflow) {
        rId = purge(corresponding, key);
        target = corresponding;
    }

    if (rId == -1) {
        system("cls");
        printf("C/A. %d not found.\n", key);
        printf("\nPress ENTER to return to the main menu");
        getchar();

        return false;
    }

    deleteClient(rId);

    for (i = 0; i < 4; i++)  // Checks if the bucket whose record has been removed is empty.
        if (target->bkt->rec[i].key != -1) {
            hasRecords = true;
            break;
        }


    /* If it is empty, merge this bucket with its corresponding,
     * '100' with '000', for example, by decreasing local depth. */
    if (!hasRecords && target->bkt->localDepth > 2) {
        corresponding = getCorrespondingDir(target);

        if (corresponding != NULL) {
            tempBucket = target->bkt;
            corresponding->bkt->localDepth--;
            target->bkt = corresponding->bkt;

            free(tempBucket);
        }
    }

    /* If all local depths are smaller than the global depth,
     * it returns the directories to the previous state
     * (decrement of the global page). */
    for (i = 0; i < dirSize; i++)
        if (dir[i].bkt->localDepth == globalDepth)
            return true;

    globalDepth--;
    dirSize = (int) pow(2, (double) globalDepth);

    dir = (directory *) realloc(dir, dirSize * sizeof(directory));
    for (i = 0; i < dirSize; i++)
        dir[i].key = toBinary(i, globalDepth);

    return true;
}

/* Access the database and returns the client that references the key by their account number. */
client *findRecord(int key) {
    int rId = 0, index = 0;
    directory *corresponding = NULL;
    client *cli = NULL;

    index = getHashCode(key, globalDepth);
    rId = find(&dir[index], key);

    corresponding = getCorrespondingDir(&dir[index]);
    if (rId == -1 && superOverflow)
        rId = find(corresponding, key);

    if (rId == -1)
        return NULL;

    cli = findClient(rId);

    return cli;
}

void initializeDirectory() {
    int i = 0;

    dirSize = (int) pow(2, (double) globalDepth);

    dir = (directory *) malloc(dirSize * sizeof(directory));
    for (i = 0; i < dirSize; i++) {
        dir[i].key = toBinary(idCtrl, globalDepth);
        dir[i].bkt = createBucket(idCtrl++, globalDepth);
    }
}

/* Print the hash table. */
void showHash() {
    int i = 0, j = 0;

    system("cls");
    printf("\n========================================================================================\n");
    printf("Record format: account number : rId\n\n");

    printf("GLOBAL DEPTH [%d]\n", globalDepth);
    for (i = 0; i < dirSize; i++) {
        printf("\n----------------------------------------------------------------------------------------\n");
        printf("Dir. %s -> Bucket %d ->\t", dir[i].key, dir[i].bkt->id);
        for (j = 0; j < 4; j++)
            dir[i].bkt->rec[j].key != -1 ? printf("\t%d : %d\t", dir[i].bkt->rec[j].key, dir[i].bkt->rec[j].rId) : printf("");
        printf("\nLocal depth [%d]\n\n", dir[i].bkt->localDepth);
    }

    printf("Press ENTER to return to the main menu");
    getchar();
}
