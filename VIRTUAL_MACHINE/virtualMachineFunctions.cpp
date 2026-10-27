#include "virtualMachine.h"

returnValue processorInit(processor *proc, const char *fileName)
{
    struct stat fileInfo;
    if (stat(fileName, &fileInfo) == -1)
        return INTERPRETER_ERROR;

    proc->numberOfReadBytes = (size_t)fileInfo.st_size / sizeof(uint8_t);

    proc->code = (uint8_t *) calloc(proc->numberOfReadBytes, sizeof(uint8_t));
    if (proc->code == NULL)
        return INTERPRETER_ERROR;

    proc->stk = {};
    STACK_INIT(&(proc->stk), initialCapacity);
    STACK_INIT(&(proc->returnStk), initialCapacity);
    proc->RAM = (stackElem_t *) calloc(RAM_SIZE, sizeof(stackElem_t));
    if (proc->RAM == NULL)
        return INTERPRETER_ERROR;

    proc->viRAM = &proc->RAM[VIRAM_POINTER];

    return INTERPRETER_CORRECT;
}

returnValue readFromFile(processor *proc, const char *fileName)
{
    FILE *filePointer = NULL;
    if (fileOpening(&filePointer, fileName, "rb") == 1)
        return INTERPRETER_ERROR;

    fread(proc->code, sizeof(uint8_t), proc->numberOfReadBytes, filePointer);

    if (closeFile(&filePointer) == 1)
        return INTERPRETER_ERROR;

    return INTERPRETER_CORRECT;
}

returnValue processorDestroyer(processor *proc)
{
    stackDestroy(&proc->stk);
    stackDestroy(&proc->returnStk);
    free(proc->code);
    free(proc->RAM);

    exit(EXIT_SUCCESS);

    return INTERPRETER_CORRECT;
}

returnValue hlt(processor *proc)
{
    processorDestroyer(proc);

    return INTERPRETER_CORRECT;
}

returnValue push(processor *proc)
{
    stackElem_t res = 0;
    stackElem_t args = 0;
    uint8_t commandCode = proc->code[proc->IP];

    proc->IP += sizeof(uint8_t);

    if (commandCode & 0x20)
    {
        memcpy(&args, &proc->code[proc->IP], sizeof(stackElem_t));

        res += args;
        args = 0;
        proc->IP += sizeof(stackElem_t);
    }
    if (commandCode & 0x40)
    {
        memcpy(&args, &proc->code[proc->IP], sizeof(uint8_t));

        res += (stackElem_t)proc->reg[args];
        proc->IP += sizeof(uint8_t);
    }
    if (commandCode & 0x80)
    {
        res = proc->RAM[res];
    }

    stackPush(&proc->stk, res);

    return INTERPRETER_CORRECT;
}

returnValue add(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);
    stackPush(&proc->stk, a + b);

    proc->IP += sizeof(uint8_t);

    return INTERPRETER_CORRECT;
}

returnValue sub(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);
    stackPush(&proc->stk, b - a);

    proc->IP += sizeof(uint8_t);

    return INTERPRETER_CORRECT;
}

returnValue mul(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);
    stackPush(&proc->stk, a * b);

    proc->IP += sizeof(uint8_t);

    return INTERPRETER_CORRECT;
}

returnValue div(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);
    if (a != 0)
    {
        stackPush(&proc->stk,  b / a);

        proc->IP += sizeof(uint8_t);
    }
    else
    {
        fprintf(stderr, "Unable operation...\n");

        return INTERPRETER_ERROR;
    }

    return INTERPRETER_CORRECT;
}

returnValue out(processor *proc)
{
    stackElem_t value = stackPop(&proc->stk);
    printf("<<" SPEC ">>\n", value);

    proc->IP += sizeof(uint8_t);

    return INTERPRETER_CORRECT;
}

returnValue pop(processor *proc)
{
    uint8_t commandCode = proc->code[proc->IP];

    proc->IP += sizeof(uint8_t);

    stackElem_t res = 0;
    stackElem_t regIndex = 0;
    stackElem_t args = 0;

    if (commandCode & 0x40)
    {
        regIndex = proc->code[proc->IP];

        proc->IP += sizeof(uint8_t);

        res += proc->reg[regIndex];

    }
    if (commandCode & 0x20)
    {
        memcpy(&args, &proc->code[proc->IP], sizeof(stackElem_t));

        res += args;

        proc->IP += sizeof(stackElem_t);
    }

    stackElem_t valueToStore = stackPop(&proc->stk);

    if (commandCode & 0x80)
    {
        proc->RAM[res] = valueToStore;
    }
    else if (commandCode & 0x40)
    {
        proc->reg[regIndex] = valueToStore;
    }

    return INTERPRETER_CORRECT;
}

returnValue jmp(processor *proc)
{
    proc->IP += sizeof(uint8_t);

    size_t targetIP = 0;

    memcpy(&targetIP, &proc->code[proc->IP], sizeof(size_t));

    proc->IP = (size_t)targetIP;

    return INTERPRETER_CORRECT;
}

returnValue ja(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);

    if (a > b)
        jmp(proc);
    else
        proc->IP += sizeof(uint8_t) + sizeof(size_t);

    return INTERPRETER_CORRECT;
}

returnValue jae(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);

    if (a >= b)
        jmp(proc);
    else
        proc->IP += sizeof(uint8_t) + sizeof(size_t);

    return INTERPRETER_CORRECT;
}

returnValue jb(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);

    if (a < b)
        jmp(proc);
    else
        proc->IP += sizeof(uint8_t) + sizeof(size_t);

    return INTERPRETER_CORRECT;
}

returnValue jbe(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);

    if (a <= b)
        jmp(proc);
    else
        proc->IP += sizeof(uint8_t) + sizeof(size_t);

    return INTERPRETER_CORRECT;
}

returnValue je(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);

    if (a == b)
        jmp(proc);
    else
        proc->IP += sizeof(uint8_t) + sizeof(size_t);

    return INTERPRETER_CORRECT;
}

returnValue jne(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);

    if (a != b)
        jmp(proc);
    else
        proc->IP += sizeof(uint8_t) + sizeof(size_t);

    return INTERPRETER_CORRECT;
}

returnValue in(processor *proc)
{
    stackElem_t value = 0;

    if(scanf(SPEC, &value) == 1)
    {
        stackPush(&proc->stk, value);

        proc->IP += sizeof(uint8_t);
    }
    else
        return INTERPRETER_ERROR;

    return INTERPRETER_CORRECT;
}

returnValue call(processor *proc)
{
    stackPush(&proc->returnStk, (stackElem_t)(proc->IP + sizeof(uint8_t) + sizeof(size_t)));

    jmp(proc);

    return INTERPRETER_CORRECT;
}

returnValue ret(processor *proc)
{
    proc->IP = (size_t)stackPop(&proc->returnStk);

    return INTERPRETER_CORRECT;
}
