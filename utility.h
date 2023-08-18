#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "geometry.h"
#include "tgaimage.h"

extern const float EPS;
extern const TGAColor white;
extern const TGAColor red;
extern const TGAColor green;
extern const TGAColor blue;

Matrix TransToM(Vec3f trans_);
Matrix ScaleToM(Vec3f scale_);
Matrix RotateXToM(float angle);
Matrix RotateYToM(float angle);
Matrix RotateZToM(float angle);

Matrix ViewTrans(Vec3f pos, Vec3f lookat);

#endif