#ifndef STACK_H
#define STACK_H

typedef unsigned int stackElem_t;
#define SPEC "%u"

//#define STACK_DEBUG

#ifdef STACK_DEBUG
    #define ON_DBG(...) __VA_ARGS__
#else
    #define ON_DBG(...)
#endif

#ifdef STACK_DEBUG
    #define STACK_INIT(STK, INITIAL_CAPACITY) stackInit(STK, INITIAL_CAPACITY \
                       ON_DBG(,__func__, __FILE__, __LINE__))
#else
    #define STACK_INIT(STK, INITIAL_CAPACITY) stackInit(STK, INITIAL_CAPACITY)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#endif

const stackElem_t POISON = 67;
const size_t MAX = (size_t)(-1) / 2;

#ifdef STACK_DEBUG
    const stackElem_t canary1 = 0xEDA;
    const stackElem_t canary2 = 0xBEDA;
    const stackElem_t canary3 = (stackElem_t)0xE1DADEDA;
    const stackElem_t canary4 = (stackElem_t)0xBEDADEDA;
#endif

enum error
{
    CORRECT = 0,
    ZERO_STACK = -1,
    ALLOCATION_ERROR = -2,
    INCORRECT_POINTER = -3,
    INITIALIZATION_ERROR = -4,
    POP_NOT_POSSIBLE = -5,
    CANARY_DEAD = -6,
    FILE_INFO_ERROR = -7,
    HASH_ERROR = -8
};

enum returnStatus
{
    NORMAL_STATUS = 0,
    ERROR_STATUS = 1
};

struct stack_t
{
    #ifdef STACK_DEBUG
        stackElem_t stuctCanary1;
    #endif
    stackElem_t *data;
    stackElem_t *pointer;
    size_t size;
    size_t capacity;
    ON_DBG(const char *functionName; const char *fileName; int line;)
    #ifdef STACK_DEBUG
        uint64_t hash;
        stackElem_t stuctCanary2;
    #endif
};

returnStatus stackInit(stack_t *stk, size_t initialCapacity
                ON_DBG(, const char *functionName, const char *fileName, int line));
returnStatus stackPush(stack_t *stk, stackElem_t value);
stackElem_t stackPop(stack_t *stk);
returnStatus stackRecalloc(stack_t *stk, size_t newCapacity);
bool compareValues(const stackElem_t value1, const stackElem_t value2);
#ifdef STACK_DEBUG
    uint64_t recountHash(stack_t *stk);
#endif
returnStatus stackDestroy(stack_t *stk);

error stackVerify(stack_t *stk);
error essentialCheck(stack_t *stk);
#ifdef STACK_DEBUG
    error hashCheck(stack_t *stk);
    error canaryCheck(stack_t *stk);
#endif
error popPossibleCheck(stack_t *stk);
#ifdef STACK_DEBUG
    error fileInfoCheck(stack_t *stk);
#endif
error zeroStackCheck(stack_t *stk);
error allocationCheck(stackElem_t *ptr);

returnStatus stackDump(stack_t *stk, FILE *fp);
bool fileOpening(FILE **fp, const char *fileName, const char *fileFlag);
bool closeFile(FILE **fp);

returnStatus logStackError(stack_t *stk, error flag);
returnStatus printStackVerify(stack_t *stk, error flag);

#endif
