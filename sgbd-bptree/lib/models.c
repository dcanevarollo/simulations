#include "models.h"

student *createStudent(int studentId, int age, char *name, char *email, char status) {
    student *newStudent = (student *) malloc(sizeof(student));

    newStudent->name = (char *) malloc(strlen(name)+1 * sizeof(char));
    newStudent->email = (char *) malloc(strlen(email)+1 * sizeof(char));

    newStudent->studentId = studentId;
    newStudent->age = age;
    strcpy(newStudent->name, name);
    strcpy(newStudent->email, email);
    newStudent->status = status;

    return newStudent;
}
