/*
 * n64-first-person-template
 *
 * (C) 2022 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include <nusys.h>
#include "graphic.h"

/*
  The viewport structure 
  The conversion from (-1,-1,-1)-(1,1,1).  The decimal part is 2-bit.
 */
static Vp vp = {
    SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,	/* The scale factor  */
    SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,	/* Move  */
};

/*
  The initialization of RDP 
*/
Gfx setup_rdpstate[] = {
  gsDPSetRenderMode(G_RM_NOOP, G_RM_NOOP2),
  gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
  gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD, SCREEN_HT),
  gsDPSetColorDither(G_CD_BAYER),
  gsSPEndDisplayList(),
};

/*
  The initialization of RSP 
*/
Gfx setup_rspstate[] = {
  gsSPViewport(&vp),
  gsSPClearGeometryMode(0xFFFFFFFF),
  gsSPSetGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK),
  gsSPTexture(0, 0, 0, 0, G_OFF),
  gsSPEndDisplayList(),
};
