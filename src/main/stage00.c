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
#include "global.h"
#include "debug_text.h"
#include "screen.h"

#include "displayLists/building/model.inc.c"
#include "displayLists/background/model.inc.c"
#include "displayLists/skybox/model.inc.c"
#include "shape/screen_dirt_spec.c"

typedef struct Player {
  Vec3F position;
  Vec2F direction; // Direction on XZ plane;
  Vec3F viewDirection; // View vector
  float rotation_y; // Horizontal (Y axis) Rotation
  float rotation_head; // Vertical Rotation
  float view_height;
  float xzspeed;
  float strafespeed;
} Player;

Player player;

NUContData controller[4];

void shadetri(Dynamic* dynamicp);

static LookAt lookate;


Lights1 sun_light = gdSPDefLights1(  80,  80,  80, /* no ambient light */
				    255, 255, 255, /* white light */
				      0,   1,  1);

/* Make the display list and activate the task. */

float rotate = 0;

static int frameid = 0;
static int init = 0;

typedef struct ScreenDirt {
  Vec2F position;
  float alpha;
  float intensity;
} ScreenDirt;

#define SCREEN_DIRT_COUNT 64

ScreenDirt screenDirt[SCREEN_DIRT_COUNT];

u16 screenDepth = GPACK_ZDZ(G_MAXFBZ,0);
float screenDirtLvl;
float screenDirtLvlPrev;

void makeDL00(void)
{
  if(!init) {
    init = 1;
    player.position.x = 0.0f;
    player.position.y = 0.0f;
    player.position.z = 0.0f;
    player.view_height = 175.0f;
    player.rotation_y = 0.0f;
    player.rotation_head = 0.0f;

    for(int i = 0; i < SCREEN_DIRT_COUNT; i++) {
      screenDirt[i].position.x = Rand_Linear() * (SCREEN_WD - 32);
      screenDirt[i].position.y = Rand_Linear() * (SCREEN_HT - 32);
      float alphalvl = Rand_Linear();
      if(alphalvl < 0.8f) alphalvl *= 0.25f;
      screenDirt[i].alpha = alphalvl * 64.0f + 16.0f;
    }

  }
  nuContDataGetExAll(controller);
  Input_TranslateControls(&globalState.input[0], controller, &globalState.settings);

  player.rotation_head += globalState.input[0].analog_r.y * 0.04f;
  player.rotation_y += globalState.input[0].analog_r.x * 0.04f;
  player.xzspeed = globalState.input[0].analog_l.y * 10.0f;
  player.strafespeed = globalState.input[0].analog_l.x * -10.0f;

  if(globalState.input[0].button & BTN_Z) {
    player.position.y -= 1.0f;
  }
  if(globalState.input[0].button & BTN_R) {
    player.position.y += 1.0f;
  }

  float p_rot_y_cos = cosf(player.rotation_y);
  float p_rot_y_sin = sinf(player.rotation_y);
  player.direction.x = p_rot_y_cos;
  player.direction.y = p_rot_y_sin;

  float p_rot_h_cos = cosf(player.rotation_head);
  float p_rot_h_sin = sinf(player.rotation_head);
  player.viewDirection.x = p_rot_y_cos * p_rot_h_cos;
  player.viewDirection.y = p_rot_h_sin;
  player.viewDirection.z = p_rot_y_sin * p_rot_h_cos;

  player.position.x += player.direction.x * player.xzspeed + player.direction.y * player.strafespeed;
  player.position.z += player.direction.y * player.xzspeed - player.direction.x * player.strafespeed;



  /* Specify the display list buffer  */
  glistp = gfx_glist;

  guPerspectiveF(
    &gfx_dynamic.projectionF,
    &gfx_dynamic.perspNorm,
    globalState.settings.fieldOfView,
    320.0F/240.0F,
    100.0f,
    21000.0f,
    1.0f); //PERSPECTIVE_DEPTH_SCALE);
  guLookAtReflectF(gfx_dynamic.viewF.mf,
    &lookate,
    player.position.x, player.position.y + player.view_height, player.position.z,
    player.position.x + player.viewDirection.x, player.position.y + player.view_height + player.viewDirection.y, player.position.z + player.viewDirection.z,
    0.0f, 1.0f, 0.0f
  );
  guLookAtF(gfx_dynamic.viewSkyF.mf,
    0.0f, 0.0f, 0.0f,
    player.viewDirection.x, player.viewDirection.y, player.viewDirection.z,
    0.0f, 1.0f, 0.0f
  );
  //guRotate(&gfx_dynamic.modeling, rotate, 0.0F, 1.0F, 0.0F);

  guRotate(&gfx_dynamic.modeling, 0.0f, 0.0F, 1.0F, 0.0F);

  rotate += 0.1f;

  MtxF temp;
  guMtxCatF(gfx_dynamic.viewSkyF.mf, gfx_dynamic.projectionF.mf, temp.mf);
  guMtxF2L(temp.mf, &gfx_dynamic.projectionSky);

  static Vec3F sunpos = { 3175.44f, 5236.72f, 4054.82f };
  Vec3F sunpos_screen;

  Screen_ToScreenCoords(&sunpos_screen, &sunpos, &temp);

  guMtxCatF(gfx_dynamic.viewF.mf, gfx_dynamic.projectionF.mf, temp.mf);
  guMtxF2L(temp.mf, &gfx_dynamic.projection);
  
  

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
  gDPSetAlphaDither(glistp++, G_AD_NOISE );
  gSPSetLights1(glistp++, sun_light);
  gDPSetEnvColor(glistp++, 255, 255, 255, 255);
  gDPPipeSync(glistp++);

  


  gSPPerspNormalize(glistp++, gfx_dynamic.perspNorm);
  //gSPLookAt(glistp++, &lookate);

  /* The set of projection modeling matrices  */
  /*guOrtho(&gfx_dynamic.projection,
	  -(float)SCREEN_WD/2.0F, (float)SCREEN_WD/2.0F,
	  -(float)SCREEN_HT/2.0F, (float)SCREEN_HT/2.0F,
	  1.0F, 10.0F, 1.0F);*/

  /* Draw a square  */
  shadetri(&gfx_dynamic);


  // Draw 2D
  gDPSetTexturePersp(glistp++, G_TP_NONE);
  gDPSetCycleType(glistp++, G_CYC_1CYCLE);
  gDPSetRenderMode(glistp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
  gDPSetTextureFilter(glistp++, G_TF_POINT);
  gSPClearGeometryMode(glistp++, 0xFFFFFFFF);
  //gSPSetGeometryMode(glistp++, G_SHADE| G_SHADING_SMOOTH);
  gDPSetCombineLERP(glistp++,
    1,  0,  PRIMITIVE,  0, // c0
    TEXEL0,  0,  PRIMITIVE,  0, // a0
    1,  0,  PRIMITIVE,  0, // c1
    TEXEL0,  0,  PRIMITIVE,  0  // a1
  );
  
  screenDirtLvl = 0.0f;
  screenDepth = Screen_GetPixelDepthSafe(sunpos_screen.x, sunpos_screen.y);
  u16 depthProbe0 = Screen_GetPixelDepthSafe(sunpos_screen.x+6, sunpos_screen.y) == GPACK_ZDZ(G_MAXFBZ,0);
  u16 depthProbe1 = Screen_GetPixelDepthSafe(sunpos_screen.x, sunpos_screen.y+6) == GPACK_ZDZ(G_MAXFBZ,0);
  u16 depthProbe2 = Screen_GetPixelDepthSafe(sunpos_screen.x-6, sunpos_screen.y) == GPACK_ZDZ(G_MAXFBZ,0);
  u16 depthProbe3 = Screen_GetPixelDepthSafe(sunpos_screen.x, sunpos_screen.y-6) == GPACK_ZDZ(G_MAXFBZ,0);
  screenDirtLvl = ((screenDepth == GPACK_ZDZ(G_MAXFBZ,0)) + depthProbe0 + depthProbe1 + depthProbe2 + depthProbe3) / 5.0f;
  if(screenDirtLvl < 0.00025f) screenDirtLvl = 0.0f;

  if(screenDirtLvlPrev != 0.0f) {
    gDPLoadTextureBlock_4b(glistp++, screen_dirt_spec_i4, G_IM_FMT_I, 32, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    for(int i = 0; i < SCREEN_DIRT_COUNT; i++) {
        
        float dx = screenDirt[i].position.x;
        float dy = screenDirt[i].position.y;

        float ddistx = sunpos_screen.x - dx;
        float ddisty = sunpos_screen.y - dy;
        float dist = ddistx * ddistx + ddisty * ddisty;
        //#define MAX_DIRT_DIST 200.0f
        #define MAX_DIRT_DIST (200.0f * 200.0f)
        if(dist > MAX_DIRT_DIST) {
          continue;
        }
        dist /= MAX_DIRT_DIST;

        gDPSetPrimColor(glistp++, 0, 0, 255, 181, 162, screenDirt[i].alpha * (1.0f - dist) * screenDirtLvlPrev);
        gDPPipeSync(glistp++);
        gSPTextureRectangle(glistp++,
          dx * 4.0f, dy * 4.0f,
          (dx+32.0f) * 4.0f, (dy+32.0f) * 4.0f,
          G_TX_RENDERTILE,
          0<<5, 0<<5,
          1<<10, 1<<10
        );
    }
  }

  screenDirtLvlPrev += (screenDirtLvl - screenDirtLvlPrev) * 0.25f;



  DebugText_Print(0,0, "Light x: %f y: %f z: %f", sunpos_screen.x, sunpos_screen.y, sunpos_screen.z);
  DebugText_Print(0,1, "Spec x: %f y: %f a: %f", screenDirt[0].position.x, screenDirt[0].position.y, screenDirt[0].alpha);


  // Draw Debug
  DebugText_Draw(&glistp);

  /* End the construction of the display list  */
  gDPFullSync(glistp++);
  gSPEndDisplayList(glistp++);

  /* Check if all are put in the array  */
  assert(glistp - gfx_glist < GFX_GLIST_LEN);

  /* Activate the RSP task.  Switch display buffers at the end of the task. */
  nuGfxTaskStart(gfx_glist,
		 (s32)(glistp - gfx_glist) * sizeof (Gfx),
		 NU_GFX_UCODE_F3DEX_NON , NU_SC_SWAPBUFFER);
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
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->projectionSky)),
		G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling)),
		G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

  gSPDisplayList(glistp++,skybox_skybox_mesh);

  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->projection)),
		G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);

  gSPDisplayList(glistp++,building_level_mesh);

  gSPDisplayList(glistp++,background_background_mesh);
}
