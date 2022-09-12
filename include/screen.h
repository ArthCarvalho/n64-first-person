#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <nusys.h>
#include "global.h"

void Screen_ToScreenCoords(Vec3F * result, Vec3F * world, MtxF * mtx);

#endif