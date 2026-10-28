#include "../commands.h"
#include "../STACK/stack.h"
#include "assembler.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Insufficient files\n");

        return 1;
    }

    fileInfoStruc_t fileToReadInfo = {.individualLines = NULL, .name = argv[1],
                                      .filePointer = NULL, .numberOfLines = 0, .buffer = NULL};

    if (readFromFile(&fileToReadInfo) != INTERPRETER_CORRECT)
        return 1;

    dataStruc_t data = {};
    for (size_t i = 0; i < NUMBER_OF_LABELS; i++)
        data.labels[i] = (size_t)-1;

    readItemsStruc_t enteredData = {};

    if ((data.data = (uint8_t *)calloc((size_t)fileToReadInfo.numberOfLines * 5, sizeof(uint8_t))) == NULL)
        return 1;

    for (size_t i = 0; i < NUMBER_OF_PASSES; i++)
    {
        rewriteTextIntoNumber(&fileToReadInfo, &data, &enteredData);

        if (i < NUMBER_OF_PASSES - 1)
            data.dataIndex = 0;
    }

    if (writeToFile(&data, argv[2]) != INTERPRETER_CORRECT)
        return 1;

    assemblerDestroyer(&fileToReadInfo, &data);

    return 0;
}
