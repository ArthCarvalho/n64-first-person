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

#include "displayLists/building/model.inc.c"
#include "displayLists/background/model.inc.c"
#include "displayLists/skybox/model.inc.c"

typedef struct AnalogMapping {
  float l_analog_x;
  float l_analog_y;
  float r_analog_x;
  float r_analog_y;
} AnalogMapping;

typedef struct PlayerControl {
  float position_x;
  float position_y;
  float position_z;
  float height;
  float rotation_x;
  float rotation_y;

  float speed_forward;
  float speed_sideways;
} PlayerControl;

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

PlayerControl playerControl;

Player player;

#define BTN_A           A_BUTTON
#define BTN_B           B_BUTTON
#define BTN_START       START_BUTTON
#define BTN_L           L_TRIG
#define BTN_R           R_TRIG
#define BTN_Z           Z_TRIG
#define BTN_DPAD_UP     U_JPAD
#define BTN_DPAD_DOWN   D_JPAD
#define BTN_DPAD_LEFT   L_JPAD
#define BTN_DPAD_RIGHT  R_JPAD
#define BTN_C_UP        U_CBUTTONS
#define BTN_C_DOWN      D_CBUTTONS
#define BTN_C_LEFT      L_CBUTTONS
#define BTN_C_RIGHT     R_CBUTTONS


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

AnalogMapping analog_map;


void makeDL00(void)
{
  if(!init) {
    init = 1;
    playerControl.position_x = 0.0f;
    playerControl.position_y = 0.0f;
    playerControl.position_z = -1000.0f;
    playerControl.height = 175.0f;


    player.position.x = 0.0f;
    player.position.y = 0.0f;
    player.position.z = 0.0f;
    player.view_height = 175.0f;
    player.rotation_y = 0.0f;
    player.rotation_head = 0.0f;

  }
  nuContDataGetExAll(controller);
  // Convert analog sticks
  analog_map.l_analog_x = controller[0].stick_x / 128.0f;
  analog_map.l_analog_y = controller[0].stick_y / 128.0f;
  analog_map.r_analog_x = 0.0f;
  analog_map.r_analog_y = 0.0f;
  if(controller[0].button & BTN_C_UP) {
    analog_map.r_analog_y += 1.0f;
  }
  if(controller[0].button & BTN_C_DOWN) {
    analog_map.r_analog_y -= 1.0f;
  }
  
  if(controller[0].button & BTN_C_LEFT) {
    analog_map.r_analog_x += 1.0f;
  }
  if(controller[0].button & BTN_C_RIGHT) {
    analog_map.r_analog_x -= 1.0f;
  }


  playerControl.speed_forward = analog_map.r_analog_y;
  playerControl.speed_sideways = analog_map.r_analog_x;


  playerControl.rotation_y += analog_map.l_analog_x;
  playerControl.rotation_x += analog_map.l_analog_y;


  player.rotation_head += analog_map.l_analog_y * 0.04f;
  player.rotation_y += analog_map.l_analog_x * 0.04f;
  player.xzspeed = analog_map.r_analog_y * 10.0f;
  player.strafespeed = analog_map.r_analog_x * 10.0f;

  if(controller[0].button & BTN_Z) {
    player.position.y -= 1.0f;
  }
  if(controller[0].button & BTN_R) {
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
    45.0f,
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

  DebugText_Print(0,0, "Testing %f", rotate);


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
