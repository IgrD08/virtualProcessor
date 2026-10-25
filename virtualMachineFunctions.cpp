#include "commands.h"
#include "stack.h"
#include "virtualMachine.h"

returnValue processorDestroyer(processor *proc)
{
    stackDestroy(&proc->stk);
    stackDestroy(&proc->returnStk);

    free(proc->code);
    free(proc->RAM);

    return INTERPRETER_CORRECT;
}

void processorDump(processor *proc, size_t codeLen)
{
    stackDump(&proc->stk, stdout);

    for (size_t i = 0; i < codeLen; i++)
    {
        printf(SPEC, proc->code[i]);
    }

    printf("\n");

    for (size_t i = 0; i < 2 * proc->IP - 1; i++)
    {
        printf(" ");
    }

    printf("^\n");

    getchar();

    return;
}

returnValue hlt(processor *proc)
{
    processorDestroyer(proc);

    exit(EXIT_SUCCESS);

    return INTERPRETER_CORRECT;
}

returnValue push(processor *proc)
{
    stackElem_t res = 0;
    stackElem_t *args = &proc->code[proc->IP + 1];
    stackElem_t commandCode = proc->code[proc->IP];

    if (commandCode & 0x20)
    {
        res += *args;
        args++;
        proc->IP++;
    }
    if (commandCode & 0x40)
    {
        res += (stackElem_t)proc->reg[*args];
        args++;
        proc->IP++;
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

    return INTERPRETER_CORRECT;
}

returnValue sub(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);
    stackPush(&proc->stk, b - a);

    return INTERPRETER_CORRECT;
}

returnValue mul(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);
    stackPush(&proc->stk, a * b);

    return INTERPRETER_CORRECT;
}

returnValue div(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);
    if (a != 0)
    {
        stackPush(&proc->stk,  b / a);
    }
    else
    {
        fprintf(stderr, "Unable operation\n");

        return INTERPRETER_ERROR;
    }

    return INTERPRETER_CORRECT;
}

returnValue out(processor *proc)
{
    stackElem_t value = stackPop(&proc->stk);
    printf("<<" SPEC ">>\n", value);

    return INTERPRETER_CORRECT;
}

returnValue pop(processor *proc)
{
    stackElem_t res = 0;
    stackElem_t regIndex = 0;
    stackElem_t *args = &proc->code[proc->IP + 1];
    stackElem_t commandCode = proc->code[proc->IP];

    if (commandCode & 0x40)
    {
        regIndex = *args;

        res += proc->reg[regIndex];
        args++;
        proc->IP++;

    }
    if (commandCode & 0x20)
    {
        res += *args;
        args++;
        proc->IP++;
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
    proc->IP = (size_t)proc->code[proc->IP + 1];

    proc->IP--;//возникает, так как в virtualMachine в цикле каждую итерацию proc->IP увеличивается на 1

    return INTERPRETER_CORRECT;
}

returnValue ja(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);

    if (a > b)
        jmp(proc);
    else
        proc->IP++;

    return INTERPRETER_CORRECT;
}

returnValue jae(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);

    if (a >= b)
        jmp(proc);
    else
        proc->IP++;

    return INTERPRETER_CORRECT;
}

returnValue jb(processor *proc)
{
    proc->IP++;

    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);

    if (a < b)
        jmp(proc);
    else
        proc->IP++;

    return INTERPRETER_CORRECT;
}

returnValue jbe(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);

    if (a <= b)
        jmp(proc);
    else
        proc->IP++;

    return INTERPRETER_CORRECT;
}

returnValue je(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);

    if (a == b)
        jmp(proc);
    else
        proc->IP++;

    return INTERPRETER_CORRECT;
}

returnValue jne(processor *proc)
{
    stackElem_t a = stackPop(&proc->stk);
    stackElem_t b = stackPop(&proc->stk);

    if (a != b)
        jmp(proc);
    else
        proc->IP++;

    return INTERPRETER_CORRECT;
}

returnValue in(processor *proc)
{
    stackElem_t value = 0;

    if(scanf(SPEC, &value) == 1)
        stackPush(&proc->stk, value);
    else
        return INTERPRETER_ERROR;

    return INTERPRETER_CORRECT;
}

returnValue call(processor *proc)
{
    stackPush(&proc->returnStk, (stackElem_t)proc->IP + 1);

    jmp(proc);

    return INTERPRETER_CORRECT;
}

returnValue ret(processor *proc)
{
    proc->IP = (size_t)stackPop(&proc->returnStk);

    return INTERPRETER_CORRECT;
}
