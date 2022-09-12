#include "screen.h"

void Screen_ToScreenCoords(Vec3F * result, Vec3F * world, MtxF * mtx) {
  guMtxXFMF(mtx->mf, world->x, world->y, world->z, &result->x, &result->y, &result->z);
}