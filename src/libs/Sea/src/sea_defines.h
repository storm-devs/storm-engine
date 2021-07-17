#pragma once

#define G2MX_SEA 0
#define G3_SEA 1
#define R200_SEA 2

#define CUBEMAP_DELTA_TIME 50

#define SEA_FRENEL_TABLE_SIZE 512
#define SEA_COS_TABLE_SIZE 2048
#define SEA_EXP_TABLE_SIZE 256

extern void GetLineABC(CVECTOR &v1, CVECTOR &v2, float &A, float &B, float &C);
extern void RotateAroundY(float &x, float &y, float cos, float sin);
