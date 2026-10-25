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
    proc.stk = {};
    STACK_INIT(&(proc.stk), initialCapacity);
    STACK_INIT(&(proc.returnStk), initialCapacity);
    proc.RAM = (stackElem_t *) calloc(RAM_SIZE, sizeof(stackElem_t));
    proc.viRAM = &proc.RAM[4000];

    struct stat fileInfo;
    if (stat(argv[1], &fileInfo) == -1)
        return INTERPRETER_ERROR;

    proc.code = (stackElem_t *) calloc((size_t)fileInfo.st_size / sizeof(stackElem_t), sizeof(stackElem_t));

    FILE *filePointer = NULL;
    if (fileOpening(&filePointer, "numericData.asm", "rb") == 1)
        return 1;

    size_t count = fread(proc.code, sizeof(int), (size_t)fileInfo.st_size / sizeof(int), filePointer);

    if (closeFile(&filePointer) == 1)
        return 1;

    for (proc.IP = 0; proc.IP < count; proc.IP++)
    {
    //printf("COMMAND: %d\n", proc.code[proc.IP] & 0x1f);
        if ((proc.code[proc.IP] & 0x1f) >= 0 || (proc.code[proc.IP] & 0x1f) <= sizeof(*arrayOfFunctionPointers) / 8)
        {
            arrayOfFunctionPointers[proc.code[proc.IP] & 0x1f](&proc);
        }
        else
        {
            fprintf(stderr, "Segmentation fault...");

            processorDestroyer(&proc);

            exit(EXIT_FAILURE);

            return 1;
        }
        //stackDump(&proc.stk, stdout);

        // processorDump(&proc, (size_t)fileInfo.st_size / sizeof(int)); //TODO - улучшить
    }

    // for (int i = 0; i < 100; i++)
    // {
    //     printf("%d ", proc.RAM[i]);
    // }

    processorDestroyer(&proc);

    return INTERPRETER_CORRECT;
}
