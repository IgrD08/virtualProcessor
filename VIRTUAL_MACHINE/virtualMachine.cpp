#include "virtualMachine.h"

returnValue(*arrayOfFunctionPointers[])(processor *proc) = {
                                                            hlt, push, add, sub, mul, div, out, pop,
                                                            jmp, ja, jae, jb, jbe, je, jne, in, call,
                                                            ret, consoledraw, windowdraw
                                                           };

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Insufficient files\n");
        return 1;
    }

    processor proc = {};
    if (processorInit(&proc, argv[1]) != INTERPRETER_CORRECT)
        return 1;

    if (readFromFile(&proc, argv[1]) != INTERPRETER_CORRECT)
        return 1;

    for (proc.IP = 0; proc.IP < proc.numberOfReadBytes;)
    {
        if (arrayOfFunctionPointers[proc.code[proc.IP] & 0x1f](&proc) != INTERPRETER_CORRECT)
        {
            fprintf(stderr, "Unable operation...");

            processorDestroyer(&proc);

            return 1;
        }
    }

    processorDestroyer(&proc);

    return 0;
}

