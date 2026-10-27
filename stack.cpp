#include "stack.h"

returnStatus stackInit(stack_t *stk, size_t initialCapacity
                       ON_DBG(, const char *functionNameRet, const char *fileNameRet, int lineNumber))
{
    #ifdef STACK_DEBUG
        if (printStackVerify(stk, CORRECT) == ERROR_STATUS)
        {
            return ERROR_STATUS;
        }

        if (initialCapacity > MAX)
        {
            return ERROR_STATUS;
        }

        if (printStackVerify(stk, zeroStackCheck(stk)) == ERROR_STATUS)
        {
            return ERROR_STATUS;
        }
    #endif

    #ifdef STACK_DEBUG
        stk->functionName = functionNameRet;
        stk->fileName = fileNameRet;
        stk->line = lineNumber;

        if (printStackVerify(stk, fileInfoCheck(stk)) == ERROR_STATUS)
        {
            return ERROR_STATUS;
        }
    #endif

    stk->pointer = (stackElem_t*)calloc(initialCapacity + 2, sizeof(stackElem_t));

    #ifdef STACK_DEBUG
        if (printStackVerify(stk, allocationCheck(stk->pointer)) == ERROR_STATUS)
        {
            return ERROR_STATUS;
        }
    #endif

    stk->data = &stk->pointer[1];

    stk->capacity = initialCapacity;

    for (size_t i = 0; i < initialCapacity; i++)
    {
        stk->data[i] = POISON;
    }

    #ifdef STACK_DEBUG
        stk->pointer[0] = canary1;
        stk->pointer[initialCapacity + 1] = canary2;
        stk->stuctCanary1 = canary3;
        stk->stuctCanary2 = canary4;
    #endif

    #ifdef STACK_DEBUG
        stk->hash = recountHash(stk);
    #endif

    return NORMAL_STATUS;
}

returnStatus stackPush(stack_t *stk, stackElem_t value)
{
    #ifdef STACK_DEBUG
        if (printStackVerify(stk, stackVerify(stk)) == ERROR_STATUS)
        {
            return ERROR_STATUS;
        }
    #endif

    if (stk->size >= stk->capacity)
    {
        stackRecalloc(stk, stk->capacity * 2);
    }

    (stk->data)[(stk->size)++] = value;

    #ifdef STACK_DEBUG
        stk->hash = recountHash(stk);

        if (printStackVerify(stk, stackVerify(stk)) == ERROR_STATUS)
        {
            return ERROR_STATUS;
        }
    #endif

    return NORMAL_STATUS;
}

stackElem_t stackPop(stack_t *stk)
{
    #ifdef STACK_DEBUG
        if (printStackVerify(stk, stackVerify(stk)) == ERROR_STATUS)
        {
            return POISON;
        }

        if (printStackVerify(stk, popPossibleCheck(stk)) == ERROR_STATUS)
        {
            return POISON;
        }
    #endif

    stk->size--;
    stackElem_t value = (stk->data)[stk->size];
    stk->data[stk->size] = POISON;

    // if (stk->size * 4 <= stk->capacity)
    // {
    //     stackRecalloc(stk, stk->capacity / 2);
    // }

    #ifdef STACK_DEBUG
        stk->hash = recountHash(stk);

        if (printStackVerify(stk, stackVerify(stk)) == ERROR_STATUS)
        {
            return POISON;
        }
    #endif

    return value;
}

returnStatus stackRecalloc(stack_t *stk, size_t newCapacity)
{
    if (newCapacity == 0) newCapacity = 1;
    stackElem_t *newStk = (stackElem_t*)realloc(stk->pointer,
                            (newCapacity + 2) * sizeof(stackElem_t));

    #ifdef STACK_DEBUG
        if (printStackVerify(stk, allocationCheck(newStk)) == ERROR_STATUS)
        {
            return ERROR_STATUS;
        }
    #endif

    stk->pointer = newStk;

    stk->data = &(stk->pointer[1]);

    if (newCapacity > stk->capacity)
    {
        for (size_t i = stk->capacity; i < newCapacity; i++)
        {
            stk->data[i] = POISON;
        }
    }

    #ifdef STACK_DEBUG
        stk->data[newCapacity] = canary2;
    #endif

    stk->capacity = newCapacity;

    return NORMAL_STATUS;
}

returnStatus stackDump(stack_t *stk, FILE *fp)
{
    #ifdef STACK_DEBUG
        fprintf(fp, "stack \"stk1\" [%p] created by %s() %s :%d\n{\n",
               stk, stk->functionName, stk->fileName, stk->line);
    #endif

    fprintf(fp, "\tcapacity = %zu\n\tsize = %zu\n\tdata = [%p]\n\t{\n",
           stk->capacity, stk->size, stk->data);

    for (size_t i = 0; i < stk->capacity; i++)
    {
        if (i < stk->size) fprintf(fp, "\t\t*[%lu] = " SPEC, i, stk->data[i]);
        else fprintf(fp, "\t\t [%zu] = " SPEC, i, stk->data[i]
        );

        if (compareValues(stk->data[i], POISON)) fprintf(fp, " (POISON)\n");
        else fprintf(fp, "\n");
    }
    fprintf(fp, "\t}\n");

    fprintf(fp, "}\n");

    return NORMAL_STATUS;
}

bool compareValues(const stackElem_t value1, const stackElem_t value2)
{
    if (sizeof(stackElem_t) == 1)
    {
        if (*(const uint8_t*)(&value1) == *(const uint8_t*)(&value2))
            return 1;
    }
    if (sizeof(stackElem_t) == 4)
    {
        if (*(const uint32_t*)(&value1) == *(const uint32_t*)(&value2))
            return 1;
    }
    if (sizeof(stackElem_t) == 8)
    {
        if (*(const uint64_t*)(&value1) == *(const uint64_t*)(&value2))
            return 1;
    }

    return 0;
}

returnStatus stackDestroy(stack_t *stk)
{
    #ifdef STACK_DEBUG
        if (printStackVerify(stk, stackVerify(stk)) == ERROR_STATUS)
        {
            return ERROR_STATUS;
        }
    #endif

    for (size_t i = 0; i < stk->capacity; i++)
    {
        stk->data[i] = POISON;
    }

    free(stk->pointer);
    stk->pointer = NULL;

    stk->size = 0;
    stk->capacity = 0;

    #ifdef STACK_DEBUG
        stk->fileName = 0;
        stk->functionName = 0;
        stk->line = 0;
    #endif

    return NORMAL_STATUS;
}

#ifdef STACK_DEBUG
    uint64_t recountHash(stack_t *stk)
    {
        uint64_t hash = 5381;

        for (size_t i = 0; i < (stk->capacity + 2) * sizeof(stackElem_t); i++)
        {
            hash = ((hash << 5) + hash) + ((uint8_t*)(stk->pointer))[i];
        }

        uint64_t oldHash = stk->hash;
        stk->hash = 0;

        for (uint8_t i = 0; i < sizeof(stack_t); i++)
        {
            hash = ((hash << 5) + hash) + ((uint8_t*)stk)[i];
        }

        stk->hash = oldHash;

        return hash;
    }
#endif
