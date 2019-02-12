#include "file-manager.h"
#include "ext-manager.h"

memory *memoryStartPointer = NULL, *memoryEndPointer = NULL;
int idCtrl = 0;

/* Stores the iNode in the memory. */
void writeINode(iNode *noI) {
    memory *newCell = (memory *) malloc(sizeof(memory));

    newCell->noI = noI;
    if (memoryStartPointer == NULL)
        memoryStartPointer = memoryEndPointer = newCell;
    else {
        memoryEndPointer->next = newCell;
        memoryEndPointer = newCell;
    }
    memoryEndPointer->next = NULL;
}


bool showAllFiles() {
    memory *memoryPointer = NULL;

    if (memoryStartPointer != NULL) {
        memoryPointer = memoryStartPointer;
        printf("\nFILE\t\tID\n");

        while (memoryPointer != NULL) {
            printf("%s\t%d\n", memoryPointer->noI->fileName, memoryPointer->noI->fileId);

            memoryPointer = memoryPointer->next;
        }

        return true;
    }

    printf("\nNo files created yet.\n");
    sleep(2);

    return false;
}

/* Help function that creates the complet file name (with extension), e.g., "test.txt". */
char *createFileName(char *fileName, char *extension) {
    /* size of fileName + size of extension + 1 (cause a char array have a '\0' in the end). */
    char *temp = (char *) malloc((strlen(fileName)+strlen(extension)+1) * sizeof(char));

    strcpy(temp, fileName);
    strcat(temp, ".");
    strcat(temp, extension);

    return temp;
}

/* Returns the start pointer of a file. */
block *getFilePointer(iNode *noI) {
    int i = 0;
    block *temp = NULL, *filePointer = NULL;

    if (noI->fileSize > getAvailableSpace())  // Returns null if there is no disc space.
        return NULL;

    for (i = 0; i < noI->fileSize; i++) {  // Creates the block chaining of the file.
        temp = getAvailableBlock();

        temp->fileId = noI->fileId;
        temp->next = getAvailableBlock();
        temp->nextId = temp->next->id;

        if (i == 0)
            filePointer = temp;
    }

    if (temp != NULL) {
        temp->next = NULL;
        temp->nextId = -1;
    }

    return filePointer;
}

/* Returns the memory pointer so that another modules be able to write or read data. */
memory *getMemory() {
    return memoryStartPointer;
}

iNode *createINode(int id, int fileId, int fileSize, char *date, char *fileName) {
    iNode *newNoI = (iNode *) malloc(sizeof(iNode));

    date[24] = '\0'; // Disregard the '\n' of ctime() function.
    newNoI->date = (char *) malloc((strlen(date) + 1) * sizeof(char));
    newNoI->fileName = (char *) malloc((strlen(fileName) + 1) * sizeof(char));

    newNoI->id = id;
    newNoI->fileId = fileId;
    newNoI->fileSize = fileSize;
    strcpy(newNoI->date, date);
    strcpy(newNoI->fileName, fileName);
    newNoI->filePointer = getFilePointer(newNoI);

    /* If the file pointer is null it means that there is no space disc to the file.
     * Thus, returns null. */
    if (newNoI->filePointer == NULL)
        return NULL;

    writeINode(newNoI);  // Writes the new iNode in the memory.

    return newNoI;
}

file *createFile(int fileId, char *fileName, char *content, char *extension) {
    int fileSize = 0;
    time_t date = 0;
    file *newFile = (file *) malloc(sizeof(file));

    /* Controls the ids of the iNodes considering 100 + file id, e.g., file id = 1 -> iNode id = 101. */
	idCtrl = (100 + fileId);
    time(&date);  // Stores the date-time of creation of the file.

    fileSize = strlen(content);
    newFile->fileName = (char *) malloc((strlen(fileName) + 1) * sizeof(char));
    newFile->content = (char *) malloc((fileSize + 1) * sizeof(char));
    newFile->extension = (char *) malloc((strlen(extension) + 1) * sizeof(char));

    /* If the file size are bigger than 20 or smaller than 2 we force it to that limits. */
    if (fileSize > 20)
        fileSize = 20;
    else if (fileSize < 2)
        fileSize = 2;

    newFile->fileId = fileId;
    strcpy(newFile->fileName, fileName);
    strcpy(newFile->content, content);
    strcpy(newFile->extension, extension);
    /* The ctime() function converts a time_t variable to a char array variable. */
    newFile->noI = createINode(idCtrl, newFile->fileId, fileSize, ctime(&date),
                           createFileName(newFile->fileName, newFile->extension));

    if (newFile->noI == NULL) // If there is no iNode, don't return the file.
        return NULL;

    return newFile;
}

void deleteFile(int fileId) {
    memory *cell = NULL, *previousCell = NULL;
    block *fileBlock = NULL;

    /* Finds the corresponding iNode in the memory.  */
    cell = memoryStartPointer;
    while (cell != NULL && cell->noI->fileId != fileId) {
        previousCell = cell;
        cell = cell->next;
    }

    if (cell != NULL) {  // If the iNode was found the removal can be performed.
        fileBlock = cell->noI->filePointer;

        /* Alters the information on every single block of the file.
         * Note: that don't actually happen in a file system. We do this
         * here to a clearly visualization of how our disc will be
         * after a removal of a file. */

        while (fileBlock != NULL) {
            fileBlock->fileId = 0;
            fileBlock->nextId = -1;

            fileBlock = fileBlock->next;
        }

        /* On a real file system only the block pointer is cut. */
        cell->noI->filePointer = NULL;

        /* Removes the iNode of the memory. */
        if (previousCell == NULL)
            memoryStartPointer = memoryStartPointer->next;
        else if (cell->next == NULL) {
            previousCell->next = NULL;
            memoryEndPointer = previousCell;
        } else
            previousCell->next = cell->next;

        printf("\n'%s' removed with success!\n", cell->noI->fileName);
        free(cell);
    } else
        printf("\nFile not found.\n");
	
	sleep(2);
}

void showMemory() {
    int op = 0, id = 0;
    memory *cell = NULL;

    clearScreen;
    if (memoryStartPointer != NULL) {
        printf("MEMORY (INODES LIST)\n\n");

        cell = memoryStartPointer;
        while (cell != NULL) {
            printf(" ---------------------------------------\n");
            printf("| %d\t\t\t\t\t|\n|\t\t\t\t\t|\n", cell->noI->id);
            printf("| Size:\t\t%d blocks\t|\n", cell->noI->fileSize);
            printf("| Add. block 1:\t\t%d\t\t|\n", cell->noI->filePointer->id);
            printf(" ---------------------------------------\n");

            if (cell->next != NULL)
                printf("\t\t|\n\t\t|\n\t\tV\n");

            cell = cell->next;
        }

        printf("\nDo you want to view any iNode individually?\t<1> Yes <2> No\n\n> ");
        scanf("%d", &op); clearBuffer;

        switch (op) {
            case 1:
                printf("\nINode id: ");
                scanf("%d", &id); clearBuffer;

                cell = memoryStartPointer;
                while (cell != NULL && cell->noI->id != id)
                    cell = cell->next;

                if (cell != NULL) {
                    clearScreen;
                    showINode(cell->noI);
                } else {
                    printf("\nINode not found.\n\nReturning to the main menu...\n");
                    sleep(2);
                }
                break;

            case 2:
                printf("\nPress ENTER to return to the main menu");
                getchar();
                break;

            default:
                printf("\nInvalid option.\n\nReturning to the main menu...\n");
                sleep(2);
                break;
        }
    } else {
        printf("Empty memory. Create a file first.\n\n");
        printf("Press ENTER to return to the main menu");
        getchar();
    }
}

void showINode(iNode *noI) {
    block *cell = NULL;

    printf(" -------------------------------------------------------\n");
    printf("| INODE\t\t\t\t\t\t\t|\n|\t\t\t\t\t\t\t|\n");
    printf("| Id:\t\t\t%d\t\t\t\t|\n", noI->id);
    printf("| File:\t\t%s\t\t\t|\n", noI->fileName);
    printf("| File size.:\t\t%d blocks\t\t\t|\n", noI->fileSize);
    printf("| Date:\t\t%s\t|\n", noI->date);
    printf("| Add. block 1:\t\t%d\t\t\t\t|\n", noI->filePointer->id);
    printf(" -------------------------------------------------------\n\n");

    printf("FILE BLOCKS CHAINING\n");

    cell = noI->filePointer;
    while (cell != NULL) {
        printf(" ---------------\n");
        printf("| %d\t\t|\n|\t\t|\n", cell->id);
        printf("| File id:  %d\t|\n", cell->fileId);
        printf("| Next id: %d\t|\n", cell->nextId);
        printf(" ---------------\n");

        if (cell->next != NULL)
            printf("\t|\n\t|\n\tV\n");

        cell = cell->next;
    }
	
	printf("\nPress ENTER to return to the main menu");
	getchar();
}

void showFile(file *f) {
    printf(" ---------------------------------------\n");
    printf("| FILE\t\t\t\t|\n|\t\t\t\t\t|\n");
    printf("| Id:\t\t%d\t\t\t|\n", f->fileId);
    printf("| Name:\t\t%s\t\t|\n", f->fileName);
    printf("| Extension:\t%s\t\t\t|\n", f->extension);
    printf("| INode:\t%d\t\t\t|\n", f->noI->id);
    printf(" ---------------------------------------\n");

    showINode(f->noI);
}

iNode *selectINode() {
    memory *cell = NULL;
    int fileId = 0;

    if (showAllFiles()) {
        printf("\nSelect the file by his id:\n\n> ");
        scanf("%d", &fileId);

        cell = memoryStartPointer;
        while (cell != NULL && cell->noI->fileId != fileId)
            cell = cell->next;

        if (cell != NULL)
            return cell->noI;

        printf("\nInvalid id.\n");
		sleep(2);
	}

    return NULL;
}

int selectFile() {
    int fileId;

    if (showAllFiles()) {
        printf("\nSelect the file by his id:\n\n> ");
        scanf("%d", &fileId);
		
		return fileId;
    }

    return 0;
}