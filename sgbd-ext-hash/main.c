#include <stdlib.h>
#include <stdio.h>
#include "lib/models.h"
#include "lib/ext-hash.h"
#include "lib/dbam.h"

void registerClient() {
    int rId = 0, accountNumber = 0;
    char *name, entityType = '\0';
    double accountBalance = 0;
    client *newClient = NULL;

    name = (char *) malloc(21 * sizeof(char));

    do {
        accountNumber = getRandom(100, 999);
        newClient = findRecord(accountNumber);
    } while (newClient != NULL);

    system("cls");
    printf("\nClient's name: ");
    scanf("%20[^\n]", name);fflush(stdin);

    printf("\nEntity type\n<I> - Individual\n<C> - Corporate\n\n> ");
    scanf("%c", &entityType);fflush(stdin);

    printf("\nAccount balance: ");
    scanf("%lf", &accountBalance); fflush(stdin);

    rId = getRid();
    newClient = createClient(accountNumber, name, entityType, accountBalance, true);

    system("cls");
    if (insertRecord(createRecord(rId, newClient->accountNumber), newClient, false))
        printf("SUCCESS!\nClient registered.\nC/A.: %d\n", newClient->accountNumber);
    else
        printf("ERROR\nCould not register client.\n");

    printf("\nPress ENTER to return to the main menu");
    getchar();

    free(newClient);
}

void accountStatement() {
    int accountNumber = 0;
    client *cli = NULL;

    system("cls");
    printf("Account number: ");
    scanf("%d", &accountNumber); fflush(stdin);

    cli = findRecord(accountNumber);
    system("cls");
    if (cli != NULL) {
        printf("\n==================================================\n");
        printf("CLIENT INFO\n\n");
        printf("Name: %s\nC/A.: %d\nEntity %c\nBalance: R$%.2lf\n\n", cli->name, cli->accountNumber, cli->entityType, cli->accountBalance);
    } else
        printf("Client not found.\n\n");

    printf("Press ENTER to return to the main menu.");
    getchar();

    free(cli);
}

void deleteAccount() {
    int accountNumber = 0;

    system("cls");
    printf("Account number: ");
    scanf("%d",&accountNumber); fflush(stdin);
    if (deleteRecord(accountNumber)) {
        printf("SUCCESS!\nClient removed.\n");
        sleep(2);
    }
}

void openDatabas() {
    showDatabase();
}

int main() {
    int op = 0;
    bool exit = false;

    initializeDirectory();
    if (!startConnection())
        return 0;  // Closes the program if cannot establish connection with the database.

    while (!exit) {
        system("cls");
        printf("\n==================================================\n");
        printf("<1> Register client\n<2> Delete account\n<3> Account statement\n<4> Show table\n<5> Show database\n<6> Exit\n\n> ");
        scanf("%d", &op); fflush(stdin);

        switch (op) {
            case 1:
                registerClient();
                break;

            case 2:
                deleteAccount();
                break;

            case 3:
                accountStatement();
                break;

            case 4:
                showHash();
                break;

            case 5:
                openDatabas();
                break;

            case 6:
                exit = 1;
                endConnection();
                break;

            default:
                printf("Invalid option.\n");
                sleep(2);
                break;
        }
    }

    return 0;
}
