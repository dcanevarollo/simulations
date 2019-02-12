#include "ext-manager.h"

int readData() {
    FILE *database = NULL;
    int noIId = 0, fileId = 0, fileSize = 0, biggerId = 0;
    char date[25], fileName[15], line[90];

    database = fopen("database.txt", "r");

    if (database == NULL)
        return 0;

    while (fgets(line, 90, database) != NULL) {
        sscanf(line, "%d;%d;%d;%[^;];%[^\n]", &noIId, &fileId, &fileSize, date, fileName);

        createINode(noIId, fileId, fileSize, date, fileName);
        
		if (fileId > biggerId)
			biggerId = fileId;
    }
    fclose(database);

    return (biggerId + 1); // Returns the bigger id created to the id control variable.
}

void writeData() {
    FILE *database = NULL;
    memory *mem = NULL;

    database = fopen("database.txt", "w");
    mem = getMemory();

    if (database == NULL) {
        printf("An error occurred creating the database: %s.\n", strerror(errno));
        sleep(2);

        return;
    }

    while (mem != NULL) {
        fprintf(database, "%d;%d;%d;%s;%s\n", mem->noI->id, mem->noI->fileId, mem->noI->fileSize,
                mem->noI->date, mem->noI->fileName);

        mem = mem->next;
    }
    fclose(database);
}

void createExternalFile(file *f) {
    FILE *externalFIle = NULL;

    externalFIle = fopen(f->noI->fileName, "w");

    if (externalFIle == NULL) {
        printf("An error occurred creating the external file: %s.\n", strerror(errno));

        return;
    }
	
    fprintf(externalFIle, "%s", f->content);
    fclose(externalFIle);
	
    clearScreen;
    printf("'%s' was successfully created.\n\n", f->noI->fileName);
}

void openExternalFile(iNode *noI) {
    FILE *fileOpener = NULL;

    switch (OS) {
        case 'W':
            system(noI->fileName);
            break;

        case 'U':
            fileOpener = fopen("file-opener.sh", "w+");

            fprintf(fileOpener, "#/bin/bash\n\n");
            fprintf(fileOpener, "gedit %s\n", noI->fileName);
            fclose(fileOpener);

            system("chmod +x file-opener.sh");
            system("bash file-opener.sh");
            break;

        default:
            exit(EXIT_FAILURE);
    }
}