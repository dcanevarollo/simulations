#ifndef DBAM_H
#define DBAM_H

/* Database access module. */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "models.h"

bool startConnection();

int getRid();

void storeClient(int rId, client *newClient);

void deleteClient(int rId);

client *findClient(int rId);

void showDatabase();

void endConnection();

#endif
