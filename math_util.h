#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <ultra64.h>

extern const f32 sincos_lut[0x140];
#define sins(ang) (sincos_lut[(u8)(ang)])
#define coss(ang) ((sincos_lut + 0x40)[(u8)(ang)])
#define lerp(x,y,z) ((x) + ((y) - (x)) * (z))
#define abs(x) ((x) < 0 ? -(x) : (x))
#define sign(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0))
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

void InitMathUtil();

#endif
