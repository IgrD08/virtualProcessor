#include "commands.h"
#include "virtualMachine.h"
#include "raylib.h"

returnValue consoledraw(processor *proc)
{
    proc->IP++;
    size_t size = (size_t)proc->reg[proc->code[proc->IP]];

    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {

            printf("[%c]", proc->RAM[size * i + j]);
        }

        printf("\n");
    }

    return INTERPRETER_CORRECT;
}

returnValue windowdraw(processor *proc)
{
    InitWindow(WINDOW_SIZE, WINDOW_SIZE, "Videomemory");
    SetTargetFPS(60);

    // Color *pixels = (Color *)malloc(WINDOW_SIZE * WINDOW_SIZE * sizeof(Color));

    Image img = {
                    .data = proc->viRAM,
                    .width = WINDOW_SIZE,
                    .height = WINDOW_SIZE,
                    .mipmaps = 1,
                    .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
                   };

    Texture2D tex = LoadTextureFromImage(img);

    while (!WindowShouldClose())
    {
        UpdateTexture(tex, proc->viRAM);

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTexture(tex, 0, 0, WHITE);

        EndDrawing();
    }

    UnloadTexture(tex);
    // free(pixels);
    CloseWindow();

    return INTERPRETER_CORRECT;
}

