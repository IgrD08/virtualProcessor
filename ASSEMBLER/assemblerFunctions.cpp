#include "../commands.h"
#include "../STACK/stack.h"
#include "assembler.h"

returnValue readFromFile(fileInfoStruc_t *fileToReadInfo)
{
    struct stat fileInfo;
    if (stat(fileToReadInfo->name, &fileInfo) == -1)
        return INTERPRETER_ERROR;

    if ((fileToReadInfo->buffer = (char *)(calloc((size_t)fileInfo.st_size + 1, sizeof(char)))) == NULL)
        return INTERPRETER_ERROR;

    if (fileOpening(&(fileToReadInfo->filePointer), fileToReadInfo->name, "r") == 1)
        return INTERPRETER_ERROR;

    size_t realCount = fread(fileToReadInfo->buffer, sizeof(char), (size_t)fileInfo.st_size, fileToReadInfo->filePointer);

    fileToReadInfo->buffer[realCount] = '\0';

    char *pointer = fileToReadInfo->buffer;

    while ((pointer = strchr(pointer, '\n')) != NULL)
    {
        fileToReadInfo->numberOfLines++;
        pointer += 1;
    }

    pointer = fileToReadInfo->buffer;

    if ((fileToReadInfo->individualLines = (char **)calloc((size_t)fileToReadInfo->numberOfLines, sizeof(char *))) == NULL)
        return INTERPRETER_ERROR;

    int i = 1;
    fileToReadInfo->individualLines[0] = pointer;
    while ((pointer = strchr(pointer, '\n')) != NULL && i < fileToReadInfo->numberOfLines)
    {
        *pointer = '\0';
        pointer += 1;
        fileToReadInfo->individualLines[i] = pointer;
        i++;
    }

    if (closeFile(&(fileToReadInfo->filePointer)) == 1)
        return INTERPRETER_ERROR;

    return INTERPRETER_CORRECT;
}

returnValue checkForLabel(dataStruc_t *data, readItemsStruc_t *enteredData)
{
    if (enteredData->string[0] == ':')
    {
        if ((enteredData->value = atoi(&enteredData->string[1])) == 0)
            return INTERPRETER_ERROR;

        memcpy(&data->data[data->dataIndex], &data->labels[enteredData->value], sizeof(size_t));
        data->dataIndex += sizeof(size_t);
    }
    else
    {
        memcpy(&data->data[data->dataIndex], &enteredData->value, sizeof(size_t));
        data->dataIndex += sizeof(size_t);
    }

    return INTERPRETER_CORRECT;
}

returnValue choseRegister(dataStruc_t *data, readItemsStruc_t *enteredData)
{
    if (!strcmp(enteredData->string, "AX"))
        data->data[data->dataIndex] = (uint8_t)AX;
    else if (!strcmp(enteredData->string, "BX"))
        data->data[data->dataIndex] = (uint8_t)BX;
    else if (!strcmp(enteredData->string, "CX"))
        data->data[data->dataIndex] = (uint8_t)CX;
    else if (!strcmp(enteredData->string, "DX"))
        data->data[data->dataIndex] = (uint8_t)DX;
    else
    {
        fprintf(stderr, "Register does not found\n");

        return INTERPRETER_ERROR;
    }

    return INTERPRETER_CORRECT;
}

returnValue rewriteTextIntoNumber(fileInfoStruc_t *fileToReadInfo,
                                  dataStruc_t *data,
                                  readItemsStruc_t *enteredData)
{
    for (int i = 0; i < fileToReadInfo->numberOfLines; i++)
    {
        enteredData->command[0] = '\0';
        enteredData->string[0] = '\0';
        enteredData->value = 0;

        if (sscanf(fileToReadInfo->individualLines[i], "%s", enteredData->command) <= 0)
            continue;

        switch (enteredData->command[0])
        {
            case 'P':
                if (!strcmp(enteredData->command, "PUSH"))
                {
                    if (sscanf(fileToReadInfo->individualLines[i], "%*s %s", enteredData->string) <= 0)
                        break;

                    if (sscanf(fileToReadInfo->individualLines[i], "%*s" SPEC, &enteredData->value) == 1)
                    {
                        data->data[data->dataIndex] = (uint8_t)(PUSH | 0x20);
                        data->dataIndex += sizeof(uint8_t);

                        memcpy(&data->data[data->dataIndex], &enteredData->value, sizeof(stackElem_t));
                        data->dataIndex += sizeof(stackElem_t);
                    }
                    else if (sscanf(fileToReadInfo->individualLines[i], "%*s [" SPEC "]", &enteredData->value) == 1)
                    {
                        data->data[data->dataIndex] = (uint8_t)(PUSH | 0x20 | 0x80);
                        data->dataIndex += sizeof(uint8_t);

                        memcpy(&data->data[data->dataIndex], &enteredData->value, sizeof(stackElem_t));
                        data->dataIndex += sizeof(stackElem_t);
                    }
                    else if (sscanf(fileToReadInfo->individualLines[i], "%*s [%s + " SPEC "]", enteredData->string, &enteredData->value) == 2)
                    {
                        data->data[data->dataIndex] = (uint8_t)(PUSH | 0x20 | 0x40 | 0x80);
                        data->dataIndex += sizeof(uint8_t);

                        choseRegister(data, enteredData);
                        data->dataIndex += sizeof(uint8_t);

                        memcpy(&data->data[data->dataIndex], &enteredData->value, sizeof(stackElem_t));
                        data->dataIndex += sizeof(stackElem_t);
                    }
                    else if (sscanf(fileToReadInfo->individualLines[i], "%*s %s +" SPEC, enteredData->string, &enteredData->value) == 2)
                    {
                        data->data[data->dataIndex] = (uint8_t)(PUSH | 0x20 | 0x40);
                        data->dataIndex += sizeof(uint8_t);

                        choseRegister(data, enteredData);
                        data->dataIndex += sizeof(uint8_t);

                        memcpy(&data->data[data->dataIndex], &enteredData->value, sizeof(stackElem_t));
                        data->dataIndex += sizeof(stackElem_t);
                    }
                    else if (sscanf(fileToReadInfo->individualLines[i], "%*s [%[^]]s", enteredData->string) == 1)
                    {
                        data->data[data->dataIndex] = (uint8_t)(PUSH | 0x40 | 0x80);
                        data->dataIndex += sizeof(uint8_t);

                        choseRegister(data, enteredData);
                        data->dataIndex += sizeof(uint8_t);
                    }
                    else if (sscanf(fileToReadInfo->individualLines[i], "%*s %s", enteredData->string) == 1)
                    {
                        data->data[data->dataIndex] = (uint8_t)(PUSH | 0x40);
                        data->dataIndex += sizeof(uint8_t);

                        choseRegister(data, enteredData);
                        data->dataIndex += sizeof(uint8_t);
                    }
                }

                if (!strcmp(enteredData->command, "POP"))
                {
                    if (sscanf(fileToReadInfo->individualLines[i], "%*s %s", enteredData->string) <= 0)
                        break;

                    if (sscanf(fileToReadInfo->individualLines[i], "%*s [" SPEC "]", &enteredData->value) == 1)
                    {
                        data->data[data->dataIndex] = (uint8_t)(POP | 0x20 | 0x80);
                        data->dataIndex += sizeof(uint8_t);

                        memcpy(&data->data[data->dataIndex], &enteredData->value, sizeof(stackElem_t));
                        data->dataIndex += sizeof(stackElem_t);
                    }
                    else if (sscanf(fileToReadInfo->individualLines[i], "%*s [%s + " SPEC "]", enteredData->string, &enteredData->value) == 2)
                    {
                        data->data[data->dataIndex] = (uint8_t)(POP | 0x20 | 0x40 | 0x80);
                        data->dataIndex += sizeof(uint8_t);

                        choseRegister(data, enteredData);
                        data->dataIndex += sizeof(uint8_t);

                        memcpy(&data->data[data->dataIndex], &enteredData->value, sizeof(stackElem_t));
                        data->dataIndex += sizeof(stackElem_t);
                    }
                    else if (sscanf(fileToReadInfo->individualLines[i], "%*s [%[^]]s", enteredData->string) == 1)
                    {
                        data->data[data->dataIndex] = (uint8_t)(POP | 0x40 | 0x80);
                        data->dataIndex += sizeof(uint8_t);

                        choseRegister(data, enteredData);
                        data->dataIndex += sizeof(uint8_t);
                    }
                    else if (sscanf(fileToReadInfo->individualLines[i], "%*s %s", enteredData->string) == 1)
                    {
                        data->data[data->dataIndex] = (uint8_t)(POP | 0x40);
                        data->dataIndex += sizeof(uint8_t);

                        choseRegister(data, enteredData);
                        data->dataIndex += sizeof(uint8_t);
                    }
                }

                break;

            case 'A':

                if (!strcmp(enteredData->command, "ADD"))
                {
                    data->data[data->dataIndex] = (uint8_t)ADD;
                    data->dataIndex += sizeof(uint8_t);
                }

                break;

            case 'C':

                if (!strcmp(enteredData->command, "CALL"))
                {
                    data->data[data->dataIndex] = (uint8_t)CALL;
                    data->dataIndex += sizeof(uint8_t);

                    if (sscanf(fileToReadInfo->individualLines[i], "%*s %s", enteredData->string) <= 0)
                        break;

                    if (checkForLabel(data, enteredData) == INTERPRETER_ERROR)
                        break;
                }

                if (!strcmp(enteredData->command, "CONSOLEDRAW"))
                {
                    data->data[data->dataIndex] = (uint8_t)CONSOLE_DRAW;
                    data->dataIndex += sizeof(uint8_t);

                    if (sscanf(fileToReadInfo->individualLines[i], "%*s %s", enteredData->string) <= 0)
                        break;

                    choseRegister(data, enteredData);
                    data->dataIndex += sizeof(uint8_t);
                }

                break;

            case 'R':

                if (!strcmp(enteredData->command, "RET"))
                {
                    data->data[data->dataIndex] = (uint8_t)RET;
                    data->dataIndex += sizeof(uint8_t);
                }

                break;

            case 'S':

                if (!strcmp(enteredData->command, "SUB"))
                {
                    data->data[data->dataIndex] = (uint8_t)SUB;
                    data->dataIndex += sizeof(uint8_t);
                }

                break;

            case 'M':

                if (!strcmp(enteredData->command, "MUL"))
                {
                    data->data[data->dataIndex] = (uint8_t)MUL;
                    data->dataIndex += sizeof(uint8_t);
                }

                break;

            case 'D':

                if (!strcmp(enteredData->command, "DIV"))
                {
                    data->data[data->dataIndex] = (uint8_t)DIV;
                    data->dataIndex += sizeof(uint8_t);
                }

                break;

            case 'O':

                if (!strcmp(enteredData->command, "OUT_OF"))
                {
                    data->data[data->dataIndex] = (uint8_t)OUT_OF;
                    data->dataIndex += sizeof(uint8_t);
                }

                break;

            case 'J':

                if (!strcmp(enteredData->command, "JMP"))
                    data->data[data->dataIndex] = (uint8_t)JMP;
                else if (!strcmp(enteredData->command, "JA"))
                    data->data[data->dataIndex] = (uint8_t)JA;
                else if (!strcmp(enteredData->command, "JAE"))
                    data->data[data->dataIndex] = (uint8_t)JAE;
                else if (!strcmp(enteredData->command, "JB"))
                    data->data[data->dataIndex] = (uint8_t)JB;
                else if (!strcmp(enteredData->command, "JBE"))
                    data->data[data->dataIndex] = (uint8_t)JBE;
                else if (!strcmp(enteredData->command, "JE"))
                    data->data[data->dataIndex] = (uint8_t)JE;
                else if (!strcmp(enteredData->command, "JNE"))
                    data->data[data->dataIndex] = (uint8_t)JNE;

                data->dataIndex += sizeof(uint8_t);

                if (sscanf(fileToReadInfo->individualLines[i], "%*s %s", enteredData->string) <= 0)
                    break;

                if (checkForLabel(data, enteredData) == INTERPRETER_ERROR)
                    break;

                break;

            case 'H':

                if (!strcmp(enteredData->command, "HLT"))
                {
                    data->data[data->dataIndex] = (uint8_t)HLT;
                    data->dataIndex += sizeof(uint8_t);
                }

                break;

            case 'I':

                if (!strcmp(enteredData->command, "IN_OF"))
                {
                    data->data[data->dataIndex] = (uint8_t)IN_OF;
                    data->dataIndex += sizeof(uint8_t);
                }

                break;

            case 'W':

                if (!strcmp(enteredData->command, "WINDOWDRAW"))
                {
                    data->data[data->dataIndex] = (uint8_t)WINDOW_DRAW;
                    data->dataIndex += sizeof(uint8_t);
                }

                break;

            case ':':

                if ((enteredData->value = atoi(&enteredData->command[1])) == 0)
                    break;

                data->labels[enteredData->value] = data->dataIndex;

                break;

            default:
                fprintf(stderr, "No such enteredData->command\n");

                break;
        }
    }

    return INTERPRETER_CORRECT;
}

returnValue assemblerDestroyer(fileInfoStruc_t *fileToReadInfo, dataStruc_t *data)
{
    free(data->data);
    free(fileToReadInfo->individualLines);
    free(fileToReadInfo->buffer);

    return INTERPRETER_CORRECT;
}

returnValue writeToFile(dataStruc_t *data, char *fileName)
{
    FILE *filePointerWrite = NULL;
    if (fileOpening(&filePointerWrite, fileName, "wb"))
        return INTERPRETER_ERROR;

    fwrite(data->data, sizeof(uint8_t), (size_t)data->dataIndex, filePointerWrite);

    if (closeFile(&filePointerWrite))
        return INTERPRETER_ERROR;

    return INTERPRETER_CORRECT;
}
