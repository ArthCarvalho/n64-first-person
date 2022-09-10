/*
 * n64-first-person-template
 *
 * (C) 2022 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#ifndef _VIMODES_H_
#define _VIMODeS_H_

#include <nusys.h>

// Half resolution interlaced
extern OSViMode customViModeNtscHan1[]; // 320x480 Interlaced NTSC
extern OSViMode customViModeMpalHan1[];// 320x480 Interlaced MPAL
extern OSViMode customViModePalHan1[]; // 320x480 Interlaced PAL

// Other Modes
extern OSViMode custom512ViModeNtscHan1[]; // 512x480 Interlaced NTSC
extern OSViMode custom256ViModeNtscLan1[]; // 256x240 Non-Interlaced NTSC 

#endif