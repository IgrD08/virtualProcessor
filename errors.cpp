#include "stack.h"

error stackVerify(stack_t *stk)
{
    error flag = CORRECT;

    if ((flag = essentialCheck(stk)) != CORRECT) return flag;

    if ((flag = allocationCheck(stk->pointer)) != CORRECT) return flag;

    #ifdef STACK_DEBUG
        if ((flag = hashCheck(stk)) != CORRECT) return flag;

        if ((flag = canaryCheck(stk)) != CORRECT) return flag;
    #endif

    #ifdef STACK_DEBUG
        if ((flag = fileInfoCheck(stk)) != CORRECT) return flag;
    #endif

    return CORRECT;
}

error essentialCheck(stack_t *stk)
{
    if (stk == NULL)
    {
         return INCORRECT_POINTER;
    }

    #ifdef _WIN32
        MEMORY_BASIC_INFORMATION mbi;//TODO что делает
        if (VirtualQuery(stk, &mbi, sizeof(mbi)) == 0 || // 0 если в виндовс нет такого адреса
            mbi.State != MEM_COMMIT || //память физически выделена и готова к работе
            (mbi.Protect & PAGE_NOACCESS) || // память выделена, но системе жестко запрещено её читать
            (mbi.Protect & PAGE_GUARD)) //страницы использующиеся операционной системой
        {
            return INCORRECT_POINTER;
        }
    #endif

    if (stk->data == NULL || stk->size > MAX || stk->capacity > MAX)
    {
        return INITIALIZATION_ERROR;
    }

    return CORRECT;
}

#ifdef STACK_DEBUG
    error hashCheck(stack_t *stk)
    {
        if (recountHash(stk) != stk->hash)
        {
            return HASH_ERROR;
        }

        return CORRECT;
    }

    error canaryCheck(stack_t *stk)
    {
        if (!compareValues(stk->pointer[0], canary1) ||
            !compareValues(stk->pointer[stk->capacity + 1], canary2) ||
            !compareValues(stk->stuctCanary1, canary3) ||
            !compareValues(stk->stuctCanary2, canary4))
        {
            return CANARY_DEAD;
        }

        return CORRECT;
    }
#endif

error popPossibleCheck(stack_t *stk)
{
    if (stk->size == 0 || stk->capacity < stk->size || stk->capacity == 0)
    {
        return POP_NOT_POSSIBLE;
    }

    return CORRECT;
}

#ifdef STACK_DEBUG
    error fileInfoCheck(stack_t *stk)
    {
        if (stk->functionName == NULL || stk->fileName == NULL || stk->line <= 0)
        {
            return FILE_INFO_ERROR;
        }

        return CORRECT;
    }
#endif

error zeroStackCheck(stack_t *stk)
{
    if (stk->size != 0 || stk->capacity != 0)
    {
        return ZERO_STACK;
    }

    return CORRECT;
}

error allocationCheck(stackElem_t *ptr)
{
    if (ptr == NULL)
    {
        return ALLOCATION_ERROR;
    }

    return CORRECT;
}

returnStatus printStackVerify(stack_t *stk, error flag)
{
    if (flag == CORRECT && stk != NULL)
    {
        return NORMAL_STATUS;
    }

    return logStackError(stk, flag);
}

returnStatus logStackError(stack_t *stk, error flag)
{
    FILE *filepointer = NULL;
    if (fileOpening(&filepointer, "errors.log", "a") != 0)
    {
        return ERROR_STATUS;
    }

    returnStatus status = ERROR_STATUS;
    bool needDump = false;

    if (stk == NULL)
    {
        fprintf(filepointer, "ERROR: The stack was created incorrectly\n");
    }
    else
    {
        switch (flag)
        {
            case CORRECT:
                status = NORMAL_STATUS;
                break;

            case ZERO_STACK:
                fprintf(filepointer, "ERROR: The stack was created incorrectly\n");
                break;

            case ALLOCATION_ERROR:
                fprintf(filepointer, "ERROR: Memory allocation error\n");
                needDump = true;
                break;

            case INCORRECT_POINTER:
                fprintf(filepointer, "ERROR: A null pointer was passed\n");
                break;

            case INITIALIZATION_ERROR:
                fprintf(filepointer, "ERROR: An incorrectly constructed test was submitted\n");
                break;

            case POP_NOT_POSSIBLE:
                fprintf(filepointer, "ERROR: The function cannot be called\n");
                needDump = true;
                break;

            case CANARY_DEAD:
                fprintf(filepointer, "ERROR: One of canaries dead\n");
                break;

            case FILE_INFO_ERROR:
                fprintf(filepointer, "ERROR: Initialization did not complete correctly\n");
                needDump = true;
                break;

            case HASH_ERROR:
                fprintf(filepointer, "ERROR: Incorrect modification of data on the stack\n");
                break;

            default:
                break;
        }

        if (needDump)
        {
            stackDump(stk, filepointer);
        }
    }

    if (closeFile(&filepointer) != 0)
    {
        return ERROR_STATUS;
    }

    return status;
}

bool fileOpening(FILE **fp, const char *fileName, const char *fileFlag)
{
    if ((*fp = fopen(fileName, fileFlag)) == NULL)
    {
        fprintf(stderr, "ERROR: Failed to open file %s\n", fileName);

        return 1;
    }

    return 0;
}

bool closeFile(FILE **fp)
{
    if (fclose(*fp) == EOF)
    {
        fprintf(stderr, "ERROR: Failed to close file\n");

        return 1;
    }

    return 0;
}
