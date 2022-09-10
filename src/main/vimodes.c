/*
 * n64-first-person-template
 *
 * (C) 2022 Arthur Carvalho de Souza Lima
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include "vimodes.h"

OSViMode customViModeNtscHan1[] = { // 320x480 Interlaced NTSC
  0xA, // type
  { // comRegs
    0x305E, // ctrl
    0x280,  // width
    0x3E52239, // burst
    0x20C, // vSync
    0xC15, // hSync
    0xC150C15, // leap
    0x6C02EC, // hStart
    0x200, // xScale
    0, // vCurrent
  },
  {
    { // fldRegs[0]
      0x280, // origin
      0x400, // yScale
      0x2301FD, // vStart
      0xE0204, // vBurst
      0x2, // vIntr
    },
    { // fldRegs[1]
      0x500, // origin
      0x400, // yScale
      0x2501FF, // vStart
      0xE0204, // vBurst
      0x2, // vIntr
    }
  }
};



OSViMode customViModeMpalHan1[] = { // 320x480 Interlaced MPAL
  0x26, // type
  { // comRegs
    0x305E, // ctrl
    0x280,  // width
    0x4651E39, // burst
    0x20C, // vSync
    0xC10, // hSync
    0xC1C0C1C, // leap
    0x6C02EC, // hStart
    0x200, // xScale
    0, // vCurrent
  },
  {
    { // fldRegs[0]
      0x280, // origin
      0x400, // yScale
      0x2301FD, // vStart
      0xB0202, // vBurst
      0x2, // vIntr
    },
    { // fldRegs[1]
      0x500, // origin
      0x400, // yScale
      0x2501FF, // vStart
      0xE0204, // vBurst
      0x2, // vIntr
    }
  }
};

OSViMode customViModePalHan1[] = { // 320x480 Interlaced PAL
  0x18, // type
  { // comRegs
    0x305E, // ctrl
    0x280,  // width
    0x4541E3A, // burst
    0x270, // vSync
    0x170C69, // hSync
    0xC6F0C6D, // leap
    0x800300, // hStart
    0x200, // xScale
    0, // vCurrent
  },
  {
    { // fldRegs[0]
      0x280, // origin
      0x400, // yScale
      0x5D0237, // vStart
      0x9026B, // vBurst
      0x2, // vIntr
    },
    { // fldRegs[1]
      0x500, // origin
      0x400, // yScale
      0x5F0239, // vStart
      0xD0269, // vBurst
      0x2, // vIntr
    }
  }
};

OSViMode custom512ViModeNtscHan1[] = { // 512x480 Interlaced NTSC
  0xA, // type
  { // comRegs
    0x305E, // ctrl
    //0x400,  // width ???
    0x200,  // width
    0x3E52239, // burst
    0x20C, // vSync
    0xC15, // hSync
    0xC150C15, // leap
    0x6C02EC, // hStart
    0x340, // xScale
    0, // vCurrent
  },
  {
    { // fldRegs[0]
      0x0, // origin
      0x800, // yScale
      0x2301FD, // vStart
      0xE0204, // vBurst
      0x2, // vIntr
    },
    { // fldRegs[1]
      0x400, // origin
      0x800, // yScale
      0x2501FF, // vStart
      0xE0204, // vBurst
      0x2, // vIntr
    }
  }
};

OSViMode custom256ViModeNtscLan1[] = { // 256x240 Non-Interlaced NTSC
  2, // type
  {
      // comRegs
      0x311E,       // ctrl
      0x100,        // width
      0x3E52239,    // burst
      0x20D,        // vSync
      0xC15,        // hSync
      0xC150C15,    // leap
      0x6C02EC,     // hStart
      0x199,        // xScale
      0,            // vCurrent
  },
  { // fldRegs
    {
        // [0]
        0x200,    // origin
        0x400,    // yScale
        0x2501FF, // vStart
        0xE0204,  // vBurst
        2,        // vIntr
    },
    {
        // [1]
        0x200,    // origin
        0x400,    // yScale
        0x2501FF, // vStart
        0xE0204,  // vBurst
        2,        // vIntr
    } },
};