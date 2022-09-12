#include "debug_text.h"
#include <stdio.h>
#include <stdarg.h>

#define DEBUGTEXT_XCHARS 34
#define DEBUGTEXT_YCHARS 26

#define DEBUGTEXT_CHSIZEX 8
#define DEBUGTEXT_CHSIZEY 8

#define DEBUGTEXT_MARGINX 3
#define DEBUGTEXT_MARGINY 2

#define DEBUGTEXT_BUFFERSIZE (DEBUGTEXT_XCHARS * DEBUGTEXT_YCHARS)
#define DEBUGTEXT_XOFFSET (DEBUGTEXT_CHSIZEX * DEBUGTEXT_MARGINX)
#define DEBUGTEXT_YOFFSET (DEBUGTEXT_CHSIZEY * DEBUGTEXT_MARGINY)

#include "shape/debug_text.c"

int DebugText_DrawActive;
char DebugText_CharBuffer[DEBUGTEXT_BUFFERSIZE];

unsigned long * DebugText_TexturePtr = NULL;

void DebugText_ClearBuffer() {
  DebugText_DrawActive = 0;
  memset(DebugText_CharBuffer, 0, DEBUGTEXT_BUFFERSIZE);
}

void DebugText_Initialize() {
  DebugText_ClearBuffer();
}

void* proutSprintf(void* dst, const char* fmt, unsigned int size) {
    return (void*)((unsigned int)memcpy(dst, fmt, size) + size);
}

int vsprintf(char* dst, const char* fmt, va_list args) {
    s32 ret = _Printf(proutSprintf, dst, fmt, args);
    if (ret > -1) {
        dst[ret] = '\0';
    }
    return ret;
}

void DebugText_Print(unsigned char x, unsigned char y, const char *fmt, ...) {
  char buffer[128];
  va_list args;
  va_start(args, fmt);
  int stringSize = vsprintf(buffer, fmt, args);
  va_end(args);
  DebugText_DrawActive = 1;
  int offset = (DEBUGTEXT_XCHARS*y)+x;
  for(int i = 0; i < stringSize; i++) {
    if(buffer[i] == 0) {
      break;
    }
    if(buffer[i] == '\n') {
      offset += (DEBUGTEXT_XCHARS - (offset % DEBUGTEXT_XCHARS) - 1);
    }
    if(offset >= DEBUGTEXT_BUFFERSIZE) return;
    DebugText_CharBuffer[offset++] = buffer[i];
  }
  
}

void DebugText_SetDrawState(Gfx ** glistp) {
  gDPPipeSync((*glistp)++);
  gDPSetTexturePersp((*glistp)++, G_TP_NONE);
  gDPSetCycleType((*glistp)++, G_CYC_1CYCLE);
  gDPSetRenderMode((*glistp)++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
  gDPSetTextureFilter((*glistp)++, G_TF_POINT);
  gDPSetTextureLUT((*glistp)++, G_TT_NONE);
  gDPSetCombineLERP((*glistp)++,
    PRIMITIVE,  0,  TEXEL0,  0, // c0
    PRIMITIVE,  0,  TEXEL0,  0, // a0
    PRIMITIVE,  0,  TEXEL0,  0, // c1
    PRIMITIVE,  0,  TEXEL0,  0  // a1
  );
  gDPLoadTextureBlock_4b((*glistp)++, debug_font_ia4, G_IM_FMT_IA, 128, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP,
      G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
  gDPSetPrimColor((*glistp)++, 0, 0, 255, 255, 255, 255);
}

void DebugText_Draw(Gfx ** glistp) {
  if(!DebugText_DrawActive) return;

  // Set draw state, texture
  DebugText_SetDrawState(glistp);

  unsigned char * current = (unsigned char*)DebugText_CharBuffer;
  int cursor_x = DEBUGTEXT_XOFFSET;
  int cursor_y = DEBUGTEXT_YOFFSET;

  for(int y = 0; y < DEBUGTEXT_YCHARS; y++) {
    for(int x = 0; x < DEBUGTEXT_XCHARS; x++) {
      unsigned char glyph = *current;
      // ASCII char, draw!
      if(glyph > 0x20) {
        glyph -= 0x20;
        int tex_s = (glyph & 0xF) << 3;
        int tex_t = (glyph & 0xF0) >> 1;
        gSPTextureRectangle((*glistp)++, cursor_x << 2, cursor_y << 2,
                        cursor_x+8 << 2,
                        cursor_y+8 << 2, G_TX_RENDERTILE, tex_s<<5, tex_t<<5,
                        1 << 10, 1 << 10);
      }
      current++;
      cursor_x += DEBUGTEXT_CHSIZEX;
    }
    cursor_y += DEBUGTEXT_CHSIZEY;
    cursor_x = DEBUGTEXT_XOFFSET;
  }
  DebugText_ClearBuffer();
}
