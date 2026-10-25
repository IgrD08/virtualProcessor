#ifndef ASSEMBLER_H
#define ASSEMBLER_H

const int NUMBER_OF_LABELS = 20;
const int NUMBER_OF_READ_ITEMS = 10;
const size_t NUMBER_OF_PASSES = 2;

struct fileInfoStruc_t
{
    char **individualLines;
    const char *name;
    FILE *filePointer;
    int numberOfLines;
    char *buffer;
};

struct dataStruc_t
{
    int labels[NUMBER_OF_LABELS];
    int *data;
    int dataIndex;
};

struct readItemsStruc_t
{
    char command[NUMBER_OF_READ_ITEMS];
    char string[NUMBER_OF_READ_ITEMS];
    int value;
};

returnValue readFromFile(fileInfoStruc_t *fileToReadInfo);
returnValue checkForLabel(dataStruc_t *data, readItemsStruc_t *enteredData);
returnValue choseRegister(dataStruc_t *data, readItemsStruc_t *enteredData);
returnValue rewriteTextIntoNumber(fileInfoStruc_t *fileToReadInfo,
                                  dataStruc_t *data,
                                  readItemsStruc_t *enteredData);
returnValue assemblerDestroyer(fileInfoStruc_t *fileToReadInfo, dataStruc_t *data);
returnValue writeToFile(dataStruc_t *data, char *fileName);

#endif
