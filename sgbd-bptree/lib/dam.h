#ifndef DAM_H
#define DAM_H

#ifdef WIN32
    #define OS 'W'  // Windows compilation
#else
    #define OS 'U'  // Unix compilation
#endif

/* Database access control module.
 * DAM: Database Access Module. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "models.h"
#include "bpt.h"

page *startConnection(page *root);

student *find(int rId);

int getRid();

void saveStudent(int rId, student *newStudent, bool verbose);

void deleteStudent(int rId);

void showDatabase();

void endConnection();

#endif
