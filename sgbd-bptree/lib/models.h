#ifndef MODELS_H
#define MODELS_H

/* Module of organization and manipulation of our models. */

#define bool char
#define true 1
#define false 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Structure to the temporary storage of the registered students. */
typedef struct _student {
    int studentId;
    int age;
    char *name;
    char *email;
    char status;  // Active or finalized.
} student;

student *createStudent(int studentId, int age, char *name, char *email, char status);

#endif
