#include "dam.h"
#include "models.h"

/* Loads the database information at system startup. */
page *startConnection(page *root) {
    FILE *database = NULL;
    student *tempStudent = NULL;
    int rId = 0, studentId = 0, age = 0;
    char bufferLine[75] = "", name[21] = "", email[41] = "", status = '\0';

    database = fopen("database.csv", "r");
    if (database == NULL) {  // If the database file does not exists it is created.
        database = fopen("database.csv", "w");
        if (database == NULL) {
            printf("\nERROR!\nFailure in the connection with the database: %s.\n", strerror(errno));

            printf("\nPress ENTER to return to the main menu\n");
            getchar();

            endConnection();
            exit(EXIT_FAILURE);
        }

        fclose(database);

        return NULL;
    }

    while (fgets(bufferLine, 75, database) != NULL) {
        sscanf(bufferLine, "%d;%d;%[^;];%d;%[^;];%c", &rId, &studentId, name, &age, email, &status);

        tempStudent = createStudent(studentId, age, name, email, status);
        root = insertRecord(root, rId, tempStudent, false);
    }

    free(tempStudent);
    fclose(database);

    return root;
}

/* Returns a student by the rId parameter. */
student *find(int rId) {
    FILE *database;
    student *tempStudent = NULL;
    int tempRId = 0, studentId = 0, age = 0;
    char bufferLine[75] = "", name[21] = "", email[41] = "", status = '\0';

    database = fopen("database.csv", "r");
    if (database == NULL) {
        printf("\nERROR!\nFailure in the connection with the database: %s.\n", strerror(errno));

        return NULL;
    }

    while (fgets(bufferLine, 75, database) != NULL) {
        sscanf(bufferLine, "%d;%d;%[^;];%d;%[^;];%c", &tempRId, &studentId, name, &age, email, &status);

        if (tempRId == rId) {
            tempStudent = createStudent(studentId, age, name, email, status);
            break;
        }
    }

    fclose(database);

    return tempStudent;
}

/* Returns the available rId to the insertion of a new student. */
int getRid() {
    FILE *database = NULL;
    int i = 0;  // File lines counter.
    char bufferLine[75] = "";  // File buffer;

    database = fopen("database.csv", "r");
    if (database == NULL) {
        printf("\nERROR!\nFailure in the connection with the database: %s.\n", strerror(errno));

        return 0;
    }

    while (fgets(bufferLine, 75, database) != NULL)
        i++;

    fclose(database);

    return i;
}

void saveStudent(int rId, student *newStudent, bool verbose) {
    FILE *database = NULL, *temp = NULL;
    int dirty = 0, _rId = 0, _studentId = 0, _age = 0;
    char bufferLine[75] = "", _name[21] = "", _email[41] = "", _status = '\0';

    dirty = 0;  // Variable that indicates if it is an alteration on the database.
    temp = fopen("temp.csv", "w");  // Temporary database that receives the new records.
    database = fopen("database.csv", "r");
    if (database == NULL || temp == NULL) {
        printf("\nERROR!\nFailure in the connection with the database: %s.\n", strerror(errno));

        return;
    }

    while (fgets(bufferLine, 75, database) != NULL) {
        sscanf(bufferLine, "%d;%d;%[^;];%d;%[^;];%c", &_rId, &_studentId, _name, &_age, _email, &_status);

        /* If the line that was read is the line to be modified, the information
         * in the file are replaced by the information in memory. */
        if (rId == _rId) {
            fprintf(temp, "%d;%d;%s;%d;%s;%c\n", rId, newStudent->studentId, newStudent->name, newStudent->age, newStudent->email, newStudent->status);
            dirty = 1;
        } else  // Else, maintain the data contained on the database.
            fprintf(temp, "%d;%d;%s;%d;%s;%c\n", _rId, _studentId, _name, _age, _email, _status);
    }

    /* If no record is overwritten, it is a new record being inserted
     * into the database, and it will be written at the end of the file. */
    if (!dirty)
        fprintf(temp, "%d;%d;%s;%d;%s;%c\n", rId, newStudent->studentId, newStudent->name, newStudent->age, newStudent->email, newStudent->status);

    fclose(database);
    fclose(temp);

    /* Deletes the previous version of the database and rename the temporary file to be the new base.
     * Both functions below return -1 in case of error. */
    if (remove("database.csv") == -1 || rename("temp.csv", "database.csv") == -1) {
        perror("\nERROR!");

        printf("\nPress ENTER to continue\n");
        getchar();

        /* To ensure the integrity of the data, in case any errors
         * be launched the connection is terminated, as well as the system. */
        endConnection();
        exit(EXIT_FAILURE);
    }

    if (verbose)
        printf("\nSUCCESS!\nStudent registered.\n");
}

/* Removes the student from the database and the B+ tree.
 * For the records removed the 'F' flag is set. */
void deleteStudent(int rId) {
    student *deleted = NULL;

    deleted = find(rId);
    deleted->status = 'F';
    saveStudent(rId, deleted, false);

    printf("\nSUCCESS!\nStudent deleted.\n");
}

/* Viewing the current state of the database.
 * Also displays the list of registered students. */
void showDatabase() {
    FILE *database = NULL;
    student *tempStudent = NULL;
    int totalSpace = 0, studentSpace = 0, rId = 0, studentId = 0, age = 0;
    char bufferLine[75] = "", name[21] = "", email[41] = "", status = '\0';

    /* If the user wants to view the .csv file during the execution of the
     * program, just uncomment the code below: */
//    switch(OS) {
//        case 'W':
//            system("database.csv");
//            break;
//
//        case 'U':
//            database = fopen("ver_bd.sh", "w+");
//            fprintf(database, "#/bin/bash\n\ngedit database.csv\n");
//            fclose(database);
//
//            system("chmod +x ver_bd.sh");
//            system("bash ver_bd.sh");
//        break;
//
//        default:
//            exit(EXIT_FAILURE);
//    }

    database = fopen("database.csv", "r");
    if (database == NULL) {
        printf("\nERROR!\nFailure in the connection with the database: %s.\n", strerror(errno));

        return;
    }

    printf("\nSTUDENTS\nInfo of the registered students\n");
    while (fgets(bufferLine, 75, database) != NULL) {
        sscanf(bufferLine, "%d;%d;%[^;];%d;%[^;];%c", &rId, &studentId, name, &age, email, &status);

        tempStudent = createStudent(studentId, age, name, email, status);
        if (status == 'F')
            studentSpace = 0;
        else {
            printf("\n------------------------------------------------------------\n");
            printf("Id: %d\nName: %s\nEmail: %s\nAge: %d\n", tempStudent->studentId, tempStudent->name, tempStudent->email, tempStudent->age);

            studentSpace = sizeof(int) + sizeof(int) + strlen(name)*sizeof(char) + strlen(email)*sizeof(char) + sizeof(int);
            printf("Size: %d KB", studentSpace);
            printf("\n------------------------------------------------------------\n");
        }

        totalSpace += studentSpace;
    }

    printf("\n%d KB stored in disc.\n", totalSpace);
}

/* Forces the removal of the database "temp.csv", if it exists. */
void endConnection() {
    FILE *temp = NULL;
    int dirty = 0;

    temp = fopen("temp.csv", "r");
    if(temp != NULL)
        dirty = 1;

    fclose(temp);

    if (dirty)
        remove("temp.csv");
}
