#include "global.h"

static u32 sRandInt = 1;
static u32 sRandFloat;

void Rand_Seed(u32 seed) {
    sRandInt = seed;
}

float Rand_Linear() {
    sRandInt = (sRandInt * 1664525) + 1013904223;
    sRandFloat = ((sRandInt >> 9) | 0x3F800000);
    return *((f32*)&sRandFloat) - 1.0f;
}