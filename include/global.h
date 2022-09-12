#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <nusys.h>

//#define HIRESO

#define NUSYS_FRAMERATE 1 // 1 = 60 FPS, 2 = 30 FPS, 3 = 20FPS (NTSC)

/* The screen size  */
#ifdef HIRESO
#define SCREEN_HT        480
#define SCREEN_WD        320
#else
#define SCREEN_HT        240
#define SCREEN_WD        320
#endif

#define PACK_RGBA(r,g,b,a) (GPACK_RGBA5551(r, g, b, a) << 16 |	GPACK_RGBA5551(r, g, b, a))

typedef struct Vec2F {
  float x, y;
} Vec2F;

typedef struct Vec3F {
  float x, y, z;
} Vec3F;


typedef float MtxF_t[4][4];
typedef union {
    MtxF_t mf;
    struct {
        float xx, yx, zx, wx,
              xy, yy, zy, wy,
              xz, yz, zz, wz,
              xw, yw, zw, ww;
    };
} MtxF;

// Control Presets
// 0x00 - Analog: Camera, C Buttons: Move
// 0x01 - Analog: Camera, D-Pad: Move
// 0x02 - Analog: Move, C Buttons: Camera
// 0x03 - Twin Joysticks (Player 1 Analog: Move, Player 2 Analog: Camera)
// 0xFF - Customized

// Control Button Assigment Settings:
// 0 - Analog Stick
// 1 - C Buttons
// 2 - D-Pad



typedef struct GameplaySettings {
  u8 controlPreset;
  u8 movementControl;
  u8 cameraControl;
  u8 cameraSensitivity;
  u8 cameraAcceleration;
  u8 fieldOfView;
} GameplaySettings;

typedef struct GlobalState {
  GameplaySettings settings;
} GlobalState;

void Rand_Seed(u32 seed);
float Rand_Linear();

#endif