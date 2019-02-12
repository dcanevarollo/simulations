#ifndef MODELS_H
#define MODELS_H

#define bool char
#define true 1
#define false 0

#include <stdlib.h>
#include <string.h>

typedef struct _record {
    int rId;
    int key;
} record;

typedef struct _bucket {
    int id;
    int localDepth;
    record *rec;
} bucket;

typedef struct _directory {
    char *key;
    bucket *bkt;
} directory;

typedef struct _client {
    int accountNumber;
    double accountBalance;
    char *name;
    char entityType;
    bool isActive;
} client;

bucket *createBucket(int id, int localDepth);

record *createRecord(int rId, int key);

client *createClient(int accountNumber, char *name, char entityType, double accountBalance, int isActive);

#endif
