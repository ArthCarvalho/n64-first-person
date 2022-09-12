#include "screen.h"

#define DEFAULT_NU_GFX_ZBUFFER_ADDR		0x80000400

void Screen_ToScreenCoords(Vec3F * result, Vec3F * world, MtxF * mtx) {
  guMtxXFMF(mtx->mf, world->x, world->y, world->z, &result->x, &result->y, &result->z);
  float w = mtx->mf[0][3]*world->x + mtx->mf[1][3]*world->y + mtx->mf[2][3]*world->z + mtx->mf[3][3];
  result->x = (SCREEN_WD / 2) + ((result->x / w) * (SCREEN_WD / 2));
  result->y = (SCREEN_HT / 2) - ((result->y / w) * (SCREEN_HT / 2));
}

u16 Screen_GetPixelDepth(int x, int y) {
  u16 * zbuffer = (u16 *)DEFAULT_NU_GFX_ZBUFFER_ADDR;
  return zbuffer[y*SCREEN_WD + x];
}

u16 Screen_GetPixelDepthSafe(int x, int y) {
  if(x < 0 || x > SCREEN_WD) return 0;
  if(y < 0 || y > SCREEN_HT) return 0;
  return Screen_GetPixelDepth(x, y);
}