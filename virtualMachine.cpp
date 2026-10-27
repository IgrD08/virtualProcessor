#include "commands.h"
#include "stack.h"
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
    if (processorInit(&proc) != INTERPRETER_CORRECT)
        return 1;

    struct stat fileInfo;
    if (stat(argv[1], &fileInfo) == -1)
        return 1;

    proc.code = (uint8_t *) calloc((size_t)fileInfo.st_size / sizeof(uint8_t), sizeof(uint8_t));

    if (readFromFile(&proc, fileInfo) != INTERPRETER_CORRECT)
        return 1;

    for (proc.IP = 0; proc.IP < (size_t)fileInfo.st_size / sizeof(uint8_t);)
    {
        if (arrayOfFunctionPointers[proc.code[proc.IP] & 0x1f](&proc) != INTERPRETER_CORRECT)
        {
            fprintf(stderr, "Unable operation...");

            return 1;
        }

        // processorDump(&proc, (size_t)fileInfo.st_size / sizeof(int)); //TODO - улучшить
    }

    processorDestroyer(&proc);

    return 0;
}
