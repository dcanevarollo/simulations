/* File system manager 
 * Authors: Douglas Canevarollo, Douglas Brandão, Gabriel Andrey 
 * Date: 10/11/2018 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lib/file-manager.h"
#include "lib/disc-manager.h"
#include "lib/ext-manager.h"

void fileForm(int fileId) {
    file *newFile = NULL;
    char fileName[11], content[31], extension[4];

    clearScreen;
    printf("Name (max. 10 characters): ");
    scanf("%10[^\n]", fileName); clearBuffer;
    printf("Extension (max. 3 characters): ");
    scanf("%3[^\n]", extension); clearBuffer;
    printf("Content (max. 30 characters): ");
    scanf("%30[^\n]", content); clearBuffer;

    newFile = createFile(fileId, fileName, content, extension);
	
	if (newFile != NULL) {
        createExternalFile(newFile);
        showFile(newFile);
	} else {
	    clearScreen;
		printf("\nError creating the file.\n\nCheck the disc space.\n\n");
		
		printf("Press ENTER to return to the main menu");
		getchar();
	}
}

int main() {
    int op = 0, fileId, idCtrl = 0;
    bool exit = false;
    iNode *noI = NULL;

    initializeDisc();
    idCtrl = readData();  // The 'readData()' function returns the last file id created.

    if (idCtrl == 0)
        idCtrl++;

    while (!exit) {
        clearScreen;
        printf("FILE MANAGER\n\n");
        printf("<1> Create file\n<2> Open file\n<3> Delete file\n<4> Show disc state\n");
        printf("<5> Show table in memory\n<6> Show available space in disc\n<7> Show memory\n<8> Exit\n\n> ");
        scanf("%d", &op); clearBuffer;

        switch (op) {
            case 1:
                fileForm(idCtrl++);
                break;

            case 2:
                noI = selectINode();
                if (noI != NULL) {
                    clearScreen;
					clearBuffer;
                    openExternalFile(noI);
                    showINode(noI);
                }
                break;

            case 3:
                fileId = selectFile();
                if (fileId != 0)
                    deleteFile(fileId);
                break;

            case 4:
                showDisc();
                break;

            case 5:
                showTable();
                break;

            case 6:
                printf("\nAvailable space in disc: %d blocks.\n\n", getAvailableSpace());

                printf("Press ENTER to return to the main menu");
                getchar();
                break;

            case 7:
                showMemory();
                break;

            case 8:
                exit = true;
                writeData();

                printf("\nProgram terminated\n\nPress ENTER to leave");
                getchar();
                break;

            default:
                printf("Invalid option.\n");
				sleep(2);
                break;
        }
    }

    return 0;
}
