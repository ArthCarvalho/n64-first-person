#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <nusys.h>

//#define HIRESO

#define NUSYS_FRAMERATE 2 // 1 = 60 FPS, 2 = 30 FPS, 3 = 20FPS (NTSC)

/* The screen size  */
#ifdef HIRESO
#define SCREEN_HT        480
#define SCREEN_WD        320
#else
#define SCREEN_HT        240
#define SCREEN_WD        320
#endif

#define PACK_RGBA(r,g,b,a) (GPACK_RGBA5551(r, g, b, a) << 16 |	GPACK_RGBA5551(r, g, b, a))

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

#define MATH_PI 3.141592653589793238f

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

typedef struct ControllerInput {
  Vec2F analog_l;
  Vec2F analog_r;
  u16 button;
  u16 trigger;
} ControllerInput;

typedef enum {
  CONTROLTYPE_ANALOG,
  CONTROLTYPE_DPAD,
  CONTROLTYPE_CBUTTONS
} ControlType;

typedef struct ControlSettings {
  u8 useDualPads;
  u8 movementPad;
  u8 movementControl;
  u8 cameraPad;
  u8 cameraControl;
} ControlSettings;

typedef struct GameplaySettings {
  u8 controlPreset;
  ControlSettings controls;
  u8 cameraSensitivity;
  u8 cameraAcceleration;
  u8 cameraInvertY;
  u8 movementSensitivity;
  u8 movementAcceleration;
  float fieldOfView;
} GameplaySettings;

typedef struct GlobalState {
  GameplaySettings settings;
  ControllerInput input[4];
} GlobalState;

extern GlobalState globalState;
extern ControlSettings controlProfiles[];

void Rand_Seed(u32 seed);
float Rand_Linear();

void Input_TranslateControls(ControllerInput * result, NUContData * input, GameplaySettings * settings);

void Input_SetControls(GameplaySettings * settings, u8 profileId);

#endif