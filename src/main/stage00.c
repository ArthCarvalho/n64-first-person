/*
 * n64-first-person-template
 *
 * (C) 2022 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include <assert.h>
#include <nusys.h>
#include "graphic.h"

#include "shape/brew/model.inc.c"

void shadetri(Dynamic* dynamicp);

static LookAt lookate;


Lights1 sun_light = gdSPDefLights1(  80,  80,  80, /* no ambient light */
				    255, 255, 255, /* white light */
				      0,   1,  1);

/* Make the display list and activate the task. */

float rotate = 0;

static int frameid = 0;

void makeDL00(void)
{
  /* Specify the display list buffer  */
  glistp = gfx_glist;

  Mtx viewmtx;

  guPerspective(
    &gfx_dynamic.projection,
    &gfx_dynamic.perspNorm,
    22.0f,
    320.0F/240.0F,
    50.0f,
    4000.0f,
    1.0f); //PERSPECTIVE_DEPTH_SCALE);
  guLookAtReflect(&viewmtx,
    &lookate,
    0.0f, 416.0f, -1030.0f,
    0.0f, 148.0f, 0.0f,
    0.0f, 1.0f, 0.0f
  );
  guRotate(&gfx_dynamic.modeling, rotate, 0.0F, 1.0F, 0.0F);

  //guRotate(&gfx_dynamic.modeling, 85.0f, 0.0F, 1.0F, 0.0F);

  rotate += 4.0f;

  guMtxCatL(&viewmtx, &gfx_dynamic.projection, &gfx_dynamic.projection);

  /*  The initialization of RCP  */
  gfxRCPInit();

  /* Clear the frame buffer and the Z-buffer  */
  gfxClearCfb();

  /*if(frameid){
    gfxClearCfbRGBA(PACK_RGBA(255,0,0,255));
  } else { 
    gfxClearCfbRGBA(PACK_RGBA(0,0,255,255));
  }
  frameid ^= 1;*/
  
  
  gDPSetCycleType(glistp++,G_CYC_1CYCLE);
  gDPSetTextureFilter(glistp++, G_TF_BILERP);
  gDPSetTexturePersp(glistp++, G_TP_PERSP);
  gSPSetGeometryMode(glistp++, G_SHADE | G_LIGHTING | G_SHADING_SMOOTH | G_ZBUFFER | G_TEXTURE_GEN | G_CULL_BACK );
  gDPSetRenderMode(glistp++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
  gSPSetLights1(glistp++, sun_light);
  gDPSetEnvColor(glistp++, 255, 255, 255, 255);
  gDPPipeSync(glistp++);

  


  gSPPerspNormalize(glistp++, gfx_dynamic.perspNorm);
  gSPLookAt(glistp++, &lookate);

  /* The set of projection modeling matrices  */
  /*guOrtho(&gfx_dynamic.projection,
	  -(float)SCREEN_WD/2.0F, (float)SCREEN_WD/2.0F,
	  -(float)SCREEN_HT/2.0F, (float)SCREEN_HT/2.0F,
	  1.0F, 10.0F, 1.0F);*/

  /* Draw a square  */
  shadetri(&gfx_dynamic);

  /* End the construction of the display list  */
  gDPFullSync(glistp++);
  gSPEndDisplayList(glistp++);

  /* Check if all are put in the array  */
  assert(glistp - gfx_glist < GFX_GLIST_LEN);

  /* Activate the RSP task.  Switch display buffers at the end of the task. */
  nuGfxTaskStart(gfx_glist,
		 (s32)(glistp - gfx_glist) * sizeof (Gfx),
		 NU_GFX_UCODE_F3DEX , NU_SC_SWAPBUFFER);
}

/* The vertex coordinate  */
static Vtx shade_vtx[] =  {
        {        -64,  64, -5, 0, 0, 0, 0, 0xff, 0, 0xff	},
        {         64,  64, -5, 0, 0, 0, 0, 0, 0, 0xff	},
        {         64, -64, -5, 0, 0, 0, 0, 0, 0xff, 0xff	},
        {        -64, -64, -5, 0, 0, 0, 0xff, 0, 0, 0xff	},
};

/* Draw a square  */
void shadetri(Dynamic* dynamicp)
{
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->projection)),
		G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling)),
		G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

  gSPDisplayList(glistp++,brew_brew_mug_mesh);
}
