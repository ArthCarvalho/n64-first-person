#ifndef DEBUGTEXT_H
#define DEBUGTEXT_H

#include <nusys.h>

void DebugText_ClearBuffer();

void DebugText_Initialize();

void DebugText_Print(unsigned char x, unsigned char y, const char *fmt, ...);

void DebugText_Draw(Gfx ** buff);

#endif