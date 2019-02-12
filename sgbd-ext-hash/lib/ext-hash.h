#ifndef EXT_HASH_H
#define EXT_HASH_H


/* We simulate the dirty bit flag and the pin count value
 * by generating random values on it. If this value is less
 * than 25 the flag is set as true. */
#define dirtyBitChance 25
#define pinCountChance 25

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include "models.h"
#include "dbam.h"

int getRandom(int min, int max);

bool insertRecord(record *rec, client *newClient, bool hasOverflow);

bool deleteRecord(int key);

client *findRecord(int key);

void initializeDirectory();

void showHash();

#endif
