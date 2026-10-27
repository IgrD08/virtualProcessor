#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

enum commands
{
    HLT                = 0,
    PUSH               = 1,
    ADD                = 2,
    SUB                = 3,
    MUL                = 4,
    DIV                = 5,
    OUT_OF             = 6,
    POP                = 7,
    JMP                = 8,
    JA                 = 9,
    JAE                = 10,
    JB                 = 11,
    JBE                = 12,
    JE                 = 13,
    JNE                = 14,
    IN_OF              = 15,
    CALL               = 16,
    RET                = 17,
    CONSOLE_DRAW       = 18,
    WINDOW_DRAW        = 19
};

enum registerEnum
{
    AX                 = 1,
    BX                 = 2,
    CX                 = 3,
    DX                 = 4
};

enum returnValue
{
    PROGRAM_OVER        = -2,
    INTERPRETER_ERROR   = -1,
    INTERPRETER_CORRECT = 0
};

#endif
