#include "../lib/controller.h"

void standBy() {
    printf("\nPress ENTER to continue\n");
    getchar();
}

void invalid() {
    printf("Invalid operation.\n");
    standBy();
}

void abstract(page *tree) {
    showTree(tree);
    standBy();
}

void showStudents() {
    showDatabase();
    standBy();
}

int menu() {
    int op = 0;

    clearScreen;
    printf("DATABASE MANAGER SYSTEM - B+ TREE\n\n");
    printf("[1] Insert\n[2] Remove\n[3] Show student list\n[4] Abstract\n[5] Exit\n\n> ");
    scanf("%d", &op); clearBuffer;

    return op;
}

page *registerStudent(page *tree) {
    student *newStudent = NULL;
    int id = 0, age = 0;
    char name[21] = "", email[41] = "";

    printf("Id:\n> ");
    scanf("%d", &id); clearBuffer;

    printf("Name:\n> ");
    scanf("%[^\n]", name); clearBuffer;

    printf("Email:\n> ");
    scanf("%[^\n]", email); clearBuffer;

    printf("Age: \n>");
    scanf("%d", &age); clearBuffer;

    newStudent = createStudent(id, age, name, email, 'A');

    tree = insertRecord(tree, getRid(), newStudent, true);
    standBy();

    free(newStudent);

    return tree;
}

page *removeStudent(page *tree) {
    int id = 0;

    printf("Id:\n> ");
    scanf("%d", &id); clearBuffer;

    tree = deleteRecord(tree, id);
    standBy();

    return tree;
}