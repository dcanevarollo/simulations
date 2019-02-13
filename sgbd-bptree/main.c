#include "lib/controller.h"

int main() {
    page *tree = NULL;
    int op = 0;
    bool exit = false;

    tree = startConnection(tree);
    while (!exit) {
        op = menu();

        switch (op) {
            case 1:
                tree = registerStudent(tree);
                break;

            case 2:
                tree = removeStudent(tree);
                break;

            case 3:
                showStudents();
                break;

            case 4:
                abstract(tree);
                break;

            case 5:
                endConnection();
                exit = true;
                break;

            default:
                invalid();
                break;
        }
    }

    return 0;
}