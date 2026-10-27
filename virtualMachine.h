#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

struct processor
{
    stack_t      stk;
    stack_t      returnStk;
    stackElem_t  *RAM;
    stackElem_t  reg[5];
    uint8_t      *code;
    stackElem_t  *viRAM;
    size_t       IP;
};

const int initialCapacity = 10;
const int WINDOW_SIZE     = 600;
const int RAM_SIZE        = 364000;

returnValue processorInit     (processor *proc);
returnValue processorDestroyer(processor *proc);
void processorDump            (processor *proc, size_t codeLen);
returnValue readFromFile      (processor *proc, struct stat fileInfo);
returnValue hlt               (processor *proc);
returnValue push              (processor *proc);
returnValue add               (processor *proc);
returnValue sub               (processor *proc);
returnValue mul               (processor *proc);
returnValue div               (processor *proc);
returnValue out               (processor *proc);
returnValue pop               (processor *proc);
returnValue jmp               (processor *proc);
returnValue ja                (processor *proc);
returnValue jae               (processor *proc);
returnValue jb                (processor *proc);
returnValue jbe               (processor *proc);
returnValue je                (processor *proc);
returnValue jne               (processor *proc);
returnValue in                (processor *proc);
returnValue call              (processor *proc);
returnValue ret               (processor *proc);
returnValue consoledraw       (processor *proc);
returnValue windowdraw        (processor *proc);

#endif
