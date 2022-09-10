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
#include "vimodes.h"

#define	ZBUFFER_ADDR	(0x80400000-320*480*2*3)
#define CFB_HIGH_ADDR0	(0x80400000-320*480*2*2)
#define CFB_HIGH_ADDR1	(0x80400000-320*480*2*1)

/* High resolution frame buffer address array. */
u16* HighFrameBuf[2] = {
    (u16*)CFB_HIGH_ADDR0,
    (u16*)CFB_HIGH_ADDR1
};

OSViMode * customViMode;

static OSThread	IdleThread;		/* Idle thread */
static OSThread	MainThread;		/* main thread */

u64		cMainStack[NU_MAIN_STACK_SIZE/sizeof(u64)];/* boot/main thread stack */
static u64	cIdleStack[NU_IDLE_STACK_SIZE/sizeof(u64)];/* Idle thread stack */

void (*cIdleFunc)(void);		/* Idle loop callback function */

static void idle(void *arg);		/* idle function */

extern void mainproc(void *arg);		/* game main function */

void boot(void) {
    osInitialize();	/* Initialize N64OS   */
    
    /* Create and execute the Idle thread  */
    osCreateThread(&IdleThread, NU_IDLE_THREAD_ID, idle, 0,
		   (cIdleStack + NU_IDLE_STACK_SIZE/8), 10);
    osStartThread(&IdleThread);
}

static void idle(void *arg) {
  /* Initialize the CALLBACK function  */
    cIdleFunc = NULL;

    nuPiInit();
    
    /* Activate the scheduler 						*/
    /* Setting of VI is NTSC/ANTIALIASING/NON-INTERLACE/16bitPixel*/
    /* Possible to change by osViSetMode. 				*/
#ifndef HIRESO
    if (osTvType == OS_TV_MPAL) {
        nuScCreateScheduler(OS_VI_MPAL_LAN1, NUSYS_FRAMERATE);
    } else if (osTvType == OS_TV_PAL) {
        nuScCreateScheduler(OS_VI_PAL_LAN1, NUSYS_FRAMERATE);
    } else {
        nuScCreateScheduler(OS_VI_NTSC_LAN1, NUSYS_FRAMERATE);
    }
#else
    if (osTvType == OS_TV_MPAL) {
        nuScCreateScheduler(OS_VI_MPAL_HAN1, NUSYS_FRAMERATE);
    } else if (osTvType == OS_TV_PAL) {
        nuScCreateScheduler(OS_VI_PAL_HAN1, NUSYS_FRAMERATE);
    } else {
        nuScCreateScheduler(OS_VI_NTSC_HAN1, NUSYS_FRAMERATE);
    }
#endif
    /* Setting of the VI interface 					*/
    /*    Specify OS_VI_DITHER_FILTER_ON and 			*/
    /*    use the DITHER filter (Default is OFF).		*/
    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON
			   | OS_VI_GAMMA_OFF
			   | OS_VI_GAMMA_DITHER_OFF
			   | OS_VI_DIVOT_ON);

    /* Create th main thread for the application.  */
    osCreateThread(&MainThread, NU_MAIN_THREAD_ID, mainproc, (void*)NULL,
		   (cMainStack + NU_MAIN_STACK_SIZE/sizeof(u64)), NU_MAIN_THREAD_PRI);
    osStartThread(&MainThread);

    /* Lower priority of the IDLE thread and give the process to the main thread  */
    osSetThreadPri(&IdleThread, NU_IDLE_THREAD_PRI);

    /* Idle loop */
    while(1){
      if((volatile int)nuIdleFunc != NULL){
          /* Execute the idle function  */
          (*cIdleFunc)();
      }
    }
}

/* Declaration of the prototype */
void stage00(int);
void makeDL00(void);

/*------------------------
	Main
--------------------------*/
void mainproc(void *arg)
{
  /* The initialization of graphic  */
  nuGfxInit();


#ifdef HIRESO
  nuGfxTaskAllEndWait();
  if (osTvType == OS_TV_MPAL) {
    osViSetMode(customViModeMpalHan1);
  } else if (osTvType == OS_TV_PAL) {
    osViSetMode(customViModePalHan1);
  } else {
    osViSetMode(customViModeNtscHan1);
  }
  nuGfxDisplayOff();
  osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON
        | OS_VI_GAMMA_OFF
        | OS_VI_GAMMA_DITHER_OFF
        | OS_VI_DIVOT_ON);
  nuGfxSetCfb(HighFrameBuf, 2);
  nuGfxSetZBuffer((u16*)(ZBUFFER_ADDR));
#endif

  /* Register call-back  */
  nuGfxFuncSet((NUGfxFunc)stage00);
  /* The screen display ON */
  nuGfxDisplayOn();

  while(1)
    ;
}

/*-----------------------------------------------------------------------------
  The call-back function 

  pendingGfx which is passed from Nusystem as the argument of the call-back 
  function is the total number of RCP tasks that are currently processing 
  and waiting for the process. 
-----------------------------------------------------------------------------*/
void stage00(int pendingGfx)
{
  /* It provides the display process if there is no RCP task that is processing. */
  if(pendingGfx < 1)
    makeDL00();		
}

