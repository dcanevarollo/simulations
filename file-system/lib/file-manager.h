#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "disc-manager.h"

#define bool char
#define true 1
#define false 0

typedef struct _iNode {
    int id;
    int fileId;
    int fileSize;  // Number of blocks in the disk.
    char *date;
    char *fileName;
    block *filePointer;
} iNode;

/* Temporary struct that store in memory the file itself.
 * The file manager does the translation for blocks on disk. */
typedef struct _file {
    int fileId;
    char *fileName;
    char *content;  // File size is based on his content.
    char *extension;
    iNode *noI;
} file;

/* The memory on this simulation are represented as a list of cells. */
typedef struct _memory {
    iNode *noI;  // The memory have a array of iNodes.
    struct _memory *next;
} memory;

memory *getMemory();

iNode *createINode(int id, int fileId, int fileSize, char *date, char *fileName);

file *createFile(int fileId, char *fileName, char *content, char *extension);

void deleteFile(int fileId);

void showMemory();

void showINode(iNode *noI);

void showFile(file *f);

iNode *selectINode();

int selectFile();

#endif