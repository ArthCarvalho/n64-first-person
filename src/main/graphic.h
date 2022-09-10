/*
 * n64-first-person-template
 *
 * (C) 2022 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#define HIRESO

#define NUSYS_FRAMERATE 1 // 1 = 60 FPS, 2 = 30 FPS, 3 = 20FPS (NTSC)

/* The screen size  */
#ifdef HIRESO
#define SCREEN_HT        480
#define SCREEN_WD        320
#else
#define SCREEN_HT        240
#define SCREEN_WD        320
#endif

/* The maximum length of the display list of one task  */
#define GFX_GLIST_LEN     2048

#define PACK_RGBA(r,g,b,a) (GPACK_RGBA5551(r, g, b, a) << 16 |	GPACK_RGBA5551(r, g, b, a))

/*-------------------------- define structure ------------------------------ */
/* The projection-matrix structure  */
typedef struct {
  Mtx     projection;
  Mtx     modeling;
  unsigned short perspNorm;
} Dynamic;

/*-------------------------------- parameter---------------------------------*/
extern Dynamic gfx_dynamic;
extern Gfx* glistp;
extern Gfx gfx_glist[GFX_GLIST_LEN];
/*-------------------------------- function ---------------------------------*/
extern void gfxRCPInit(void);
extern void gfxClearCfb(void);
extern void gfxClearCfbRGBA(u32 pattern);
/*------------------------------- other extern define -----------------------*/
extern Gfx setup_rdpstate[];
extern Gfx setup_rspstate[];

#endif /* _GRAPHIC_H_ */



