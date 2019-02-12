#include "dbam.h"
#include "ext-hash.h"

/* Function called on the start of the program.
 * It will read the database to restore the data in there. */
bool startConnection() {
    FILE *database = NULL;
    int rId = 0, accountNumber = 0;
    double accountBalance = 0;
    char bufferLine[50] = "", name[21] = "", entityType = '\0';
    bool isActive = false;
    client *cli = NULL;

    database = fopen("database.csv", "r");
    if (database == NULL) {
        printf("ERROR!\nFailure in the connection with the database.\n");

        return false;
    }

    while (fgets(bufferLine, 50, database) != NULL) {
        sscanf(bufferLine, "%d;%d;%[^;];%c;%lf;%c", &rId, &accountNumber, name, &entityType, &accountBalance, &isActive);

        cli = createClient(accountNumber, name, entityType, accountBalance, isActive);
        insertRecord(createRecord(rId, cli->accountNumber), cli, 0);
    }

    free(cli);
    fclose(database);

    return true;
}

/* Returns the available rId to the new record that will be stored. */
int getRid() {
    FILE *database = NULL;
    int i = 0;
    char bufferLine[50] = "";

    database = fopen("database.csv", "r");
    if (database == NULL)
        return 0;

    while (fgets(bufferLine, 50, database) != NULL)
        i++;

    fclose(database);

    return i;
}

/* This function creates a temporary database that contains the new records.
 * After the complete storage this temporary database becomes the original one. */
void storeClient(int rId, client *newClient) {
    FILE *database = NULL, *temp = NULL;
    int _rId = 0, _accountNumber = 0;
    char bufferLine[50] = "", _name[21] = "", _entityType = '\0';
    bool isRemoval = false, _isActive = false;
    double _accountBalance = 0;

    temp = fopen("temp.csv", "w");
    database = fopen("database.csv", "r");
    while (fgets(bufferLine, 50, database) != NULL) {
        sscanf(bufferLine, "%d;%d;%[^;];%c;%lf;%c\n", &_rId, &_accountNumber, _name, &_entityType, &_accountBalance, &_isActive);

        if (rId == _rId) {
            fprintf(temp, "%d;%d;%s;%c;%.2lf;%c\n", rId, newClient->accountNumber, newClient->name, newClient->entityType, newClient->accountBalance, newClient->isActive);
            isRemoval = true;
        } else
            fprintf(temp, "%d;%d;%s;%c;%.2lf;%c\n", _rId, _accountNumber, _name, _entityType, _accountBalance, _isActive);
    }

    if (!isRemoval)
        fprintf(temp, "%d;%d;%s;%c;%.2lf;%c\n", rId, newClient->accountNumber, newClient->name, newClient->entityType, newClient->accountBalance, newClient->isActive);

    fclose(database);
    fclose(temp);

    remove("database.csv");
    rename("temp.csv", "database.csv");
}

void deleteClient(int rId) {
    client *deleted = NULL;

    deleted = findClient(rId);
    deleted->isActive = false;
    storeClient(rId, deleted);
}

void showDatabase() {
    system("database.csv");
}

client *findClient(int rId) {
    FILE *database = NULL;
    int ridTemp = 0, accountNumber = 0;
    char bufferLine[50] = "", name[21] = "", entityType = '\0';
    bool isActive = false;
    double accountBalance = 0;
    client *cli = NULL;

    database = fopen("database.csv", "r");
    if (database == NULL) {
        printf("ERROR!\nFailure in the connection with the database.\n");

        return NULL;
    }

    while (fgets(bufferLine, 50, database) != NULL) {
        sscanf(bufferLine, "%d;%d;%[^;];%c;%lf;%c", &ridTemp, &accountNumber, name, &entityType, &accountBalance, &isActive);

        if (ridTemp == rId) {
            cli = createClient(accountNumber, name, entityType, accountBalance, isActive);
            break;
        }
    }

    free(cli);
    fclose(database);

    return cli;
}

/* If the temporary database cannot be removed this function forces that removal. */
void endConnection() {
    FILE *temp = NULL;
    bool removal = false;

    temp = fopen("temp.csv", "r");
    if (temp != NULL)
        removal = true;

    fclose(temp);

    if (removal)
        remove("temp.csv");
}
