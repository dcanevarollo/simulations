#include "models.h"

bucket *createBucket(int id, int localDepth) {
    int i = 0;
    bucket *newBucket = (bucket *) malloc(sizeof(bucket));

    newBucket->id = id;
    newBucket->localDepth = localDepth;
    newBucket->rec = (record *) malloc(4 * sizeof(record));

    for (i = 0; i < 4; i++) {
        newBucket->rec[i].key = -1;
        newBucket->rec[i].rId = -1;
    }

    return newBucket;
}

/* The rId value represents the line that the record is inserted in the database file. */
record *createRecord(int rId, int key) {
    record *newRecord = (record *) malloc(sizeof(record));

    newRecord->rId = rId;
    newRecord->key = key;

    return newRecord;
}

/* Instances of this structure will be deleted of the memory after the
 * correct storage in the database file. */
client *createClient(int accountNumber, char *name, char entityType, double accountBalance, int isActive) {
    client *newClient = (client *) malloc(sizeof(client));

    newClient->name = (char *) malloc(strlen(name)+1 * sizeof(char));

    newClient->accountNumber = accountNumber;
    strcpy(newClient->name, name);
    newClient->entityType = entityType;
    newClient->accountBalance = accountBalance;
    newClient->isActive = isActive;

    return newClient;
}
