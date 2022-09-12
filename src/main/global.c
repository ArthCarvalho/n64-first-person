#include "global.h"

static u32 sRandInt = 1;
static u32 sRandFloat;

GlobalState globalState;

ControlSettings controlProfiles[] = {
  // 0 - Default Profile, Single Pad, Analog: Camera, C-Buttons: Move
  {
    0, // Single gamepad
    0, // Movement Gamepad
    CONTROLTYPE_CBUTTONS, // Use C-Buttons
    0, // Camera Gamepad
    CONTROLTYPE_ANALOG // Use Analog Stick
  },
  // 1 - Single Pad, Analog: Move, C-Buttons: Camera
  {
    0, // Single gamepad
    0, // Movement Gamepad
    CONTROLTYPE_ANALOG, // Use Analog Stick
    0, // Camera Gamepad
    CONTROLTYPE_CBUTTONS // Use C-Buttons
  },
  // 2 - Single Pad, Analog: Move, D-Pad: Camera
  {
    0, // Single gamepad
    0, // Movement Gamepad
    CONTROLTYPE_DPAD, // Use Analog Stick
    0, // Camera Gamepad
    CONTROLTYPE_ANALOG // Use C-Buttons
  },
  // 3 - Dual Pad, 1p Analog: Move, 2p Analog: Camera
  {
    1, // Dual gamepad
    0, // Movement Gamepad
    CONTROLTYPE_ANALOG, // Use Analog Stick
    1, // Camera Gamepad
    CONTROLTYPE_ANALOG, // Use Analog Stick
  },
  // 4 - Inverse Dual Pad, 1p Analog: Camera, 2p Analog: Move
  {
    1, // Dual gamepad
    1, // Movement Gamepad
    CONTROLTYPE_ANALOG, // Use Analog Stick
    0, // Camera Gamepad
    CONTROLTYPE_ANALOG, // Use Analog Stick
  },
  // 5 - Single Pad, D-pad: Move, C-Buttons: Camera
  {
    0, // Single gamepad
    0, // Movement Gamepad
    CONTROLTYPE_DPAD, // Use Analog Stick
    0, // Camera Gamepad
    CONTROLTYPE_CBUTTONS // Use C-Buttons
  },
};

void Rand_Seed(u32 seed) {
    sRandInt = seed;
}

float Rand_Linear() {
    sRandInt = (sRandInt * 1664525) + 1013904223;
    sRandFloat = ((sRandInt >> 9) | 0x3F800000);
    return *((f32*)&sRandFloat) - 1.0f;
}

void Input_GetAxis(Vec2F * result, NUContData * input, u8 type) {
  switch(type) {
    default:
    case CONTROLTYPE_ANALOG:
      result->x = input->stick_x;
      result->y = input->stick_y;
    break;
    case CONTROLTYPE_DPAD:
      if(input->button & BTN_DPAD_LEFT) {
        result->x -= 128.0f;
      }
      if(input->button & BTN_DPAD_RIGHT) {
        result->x += 128.0f;
      }
      if(input->button & BTN_DPAD_UP) {
        result->y += 128.0f;
      }
      if(input->button & BTN_DPAD_DOWN) {
        result->y -= 128.0f;
      }
    break;
    case CONTROLTYPE_CBUTTONS:
      if(input->button & BTN_C_LEFT) {
        result->x -= 128.0f;
      }
      if(input->button & BTN_C_RIGHT) {
        result->x += 128.0f;
      }
      if(input->button & BTN_C_UP) {
        result->y += 128.0f;
      }
      if(input->button & BTN_C_DOWN) {
        result->y -= 128.0f;
      }
    break;
  }
}

#define BUTTON_INPUT_MASK ( BTN_A | BTN_B | BTN_START | BTN_L | BTN_R | BTN_Z | \
U_JPAD | D_JPAD | L_JPAD | R_JPAD | \
U_CBUTTONS | D_CBUTTONS | L_CBUTTONS | R_CBUTTONS )

void Input_TranslateControls(ControllerInput * result, NUContData * input, GameplaySettings * settings) {
  NUContData * icMove = &input[settings->controls.movementPad];
  NUContData * icCamera = &input[settings->controls.cameraPad];

  Input_GetAxis(&result->analog_l, icMove, settings->controls.movementControl);
  Input_GetAxis(&result->analog_r, icCamera, settings->controls.cameraControl);

  result->analog_l.x /= 128.0f;
  result->analog_l.y /= 128.0f;
  result->analog_r.x /= 128.0f;
  result->analog_r.y /= 128.0f;

  if(settings->cameraInvertY) {
    result->analog_r.y *= -1.0f;
  }

  u16 inputMask = BUTTON_INPUT_MASK;
  // Check if either D-Pad or C-Buttons is assigned to something else and mask them out
  if(settings->controls.movementControl == CONTROLTYPE_DPAD || settings->controls.cameraControl == CONTROLTYPE_DPAD) {
    inputMask |= ~(U_JPAD | D_JPAD | L_JPAD | R_JPAD);
  }
  if(settings->controls.movementControl == CONTROLTYPE_CBUTTONS || settings->controls.cameraControl == CONTROLTYPE_CBUTTONS) {
    inputMask |= ~(U_CBUTTONS | D_CBUTTONS | L_CBUTTONS | R_CBUTTONS);
  }

  if(settings->controls.useDualPads) {
    // Get button presses from both controllers
    result->button = (icMove->button | icCamera->button) & inputMask;
    result->trigger = (icMove->trigger | icCamera->trigger) & inputMask;
  } else {
    result->button = icMove->button & inputMask;
    result->trigger = (icMove->trigger | icCamera->trigger) & inputMask;
  }
  // TODO: Change analog stick response curve and other options here.
}

void Input_SetControls(GameplaySettings * settings, u8 profileId) {
  settings->controlPreset = profileId;
  settings->controls = controlProfiles[settings->controlPreset];
}