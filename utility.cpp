#include "utility.h"

extern const float EPS = 1e-4;
extern const TGAColor white(255,255,255,255);
extern const TGAColor red(255,0,0,255);
extern const TGAColor green(0,255,0,255);
extern const TGAColor blue(0,0,255,255);

Matrix TransToM(Vec3f trans_)
{
    Matrix trans;
    for(int i=0;i<3;i++)
    {
        trans[i][i] = 1;
        trans[i][3] = trans_[i];
    }
    trans[3][3] = 1;
    return trans;
}
Matrix ScaleToM(Vec3f scale_)
{
    Matrix scale;
    for(int i=0;i<3;i++)
    {
        scale[i][i] = scale_[i];
    }
    scale[3][3] = 1;
    return scale;
}
Matrix RotateXToM(float angle)
{
    angle *= PI/180.0;
    Matrix rotateX;
    rotateX[0][0] = 1;
    rotateX[1][1] = std::cos(angle);
    rotateX[1][2] = -std::sin(angle);
    rotateX[2][1] = std::sin(angle);
    rotateX[2][2] = std::cos(angle);
    rotateX[3][3] = 1;
    return rotateX;
}
Matrix RotateYToM(float angle)
{
    angle *= PI/180.0;
    Matrix rotateY;
    rotateY[1][1] = 1;
    rotateY[0][0] = std::cos(angle);
    rotateY[0][2] = std::sin(angle);
    rotateY[2][0] = -std::sin(angle);
    rotateY[2][2] = std::cos(angle);
    rotateY[3][3] = 1;
    return rotateY;
}
Matrix RotateZToM(float angle)
{
    angle *= PI/180.0;
    Matrix rotateZ;
    rotateZ[2][2] = 1;
    rotateZ[0][0] = std::cos(angle);
    rotateZ[0][1] = -std::sin(angle);
    rotateZ[1][0] = std::sin(angle);
    rotateZ[1][1] = std::cos(angle);
    rotateZ[3][3] = 1;
    return rotateZ;
}
Matrix ViewTrans(Vec3f pos, Vec3f lookat)
{
    Vec3f _z = (lookat*-1).normalize();
    Vec3f _x = (_z ^ Vec3f(0,1,0) ).normalize();
    Vec3f _y = (_x ^ _z).normalize();

    Matrix viewTrans;
    for(int i=0;i<3;i++)
    {
        viewTrans[i][0] = _x[i];
        viewTrans[i][1] = _y[i];
        viewTrans[i][2] = _z[i];
        viewTrans[i][3] = pos[i];
    }
    viewTrans[3][3] = 1;
    return viewTrans.inverse();
}