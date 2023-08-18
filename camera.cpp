#include "camera.h"

Camera::Camera(Vec3f pos_, Vec3f lookat_, float near_, float far_, float aspect_):
                near(near_), far(far_), aspect(aspect_), BaseObject(pos_), lookat(lookat_)
{
    this->z_ = (lookat*-1).normalize();
    this->x_ = (z_ ^ Vec3f(0,1,0) ).normalize();
    this->y_ = (x_ ^ z_).normalize();
}

Vec3f Camera::GetXAxis()
{
    return x_;
}
Vec3f Camera::GetYAxis()
{
    return y_;
}
Vec3f Camera::GetZAxis()
{
    return z_;
}
Vec3f Camera::GetPos()
{
    return transform.position;
}
float Camera::GetNear()
{
    return near;
}
float Camera::GetFar()
{
    return far;
}
float Camera::GetAspect()
{
    return aspect;
}
Matrix Camera::ViewTrans()
{
    Matrix viewTrans;
    for(int i=0;i<3;i++)
    {
        viewTrans[i][0] = x_[i];
        viewTrans[i][1] = y_[i];
        viewTrans[i][2] = z_[i];
        viewTrans[i][3] = transform.position[i];
    }
    viewTrans[3][3] = 1;
    return viewTrans.inverse();
}

OrthoCamera::OrthoCamera(Vec3f pos_, Vec3f lookat_, float height_, float near_, float far_, float aspect_):
             Camera(pos_,lookat_,near_,far_,aspect_), height(height_) {}

Matrix OrthoCamera::ProjectTrans()
{
    float width = aspect * height;
    Matrix trans_ = TransToM(Vec3f(0, 0, -(far+near)/2));
    Matrix scale_ = ScaleToM(Vec3f(2.0/width, 2.0/height, 2.0/(near-far)));
    return scale_ * trans_;
}

PerspCamera::PerspCamera(Vec3f pos_, Vec3f lookat_, float FOV_, float near_, float far_, float aspect_):
             Camera(pos_,lookat_,near_,far_,aspect_), FOV(FOV_) {}

Matrix PerspCamera::ProjectTrans()
{
    float height = std::abs(2 * near * std::tan(FOV*PI/360.0));
    Matrix P2O;
    P2O[0][0] = near;
    P2O[1][1] = near;
    P2O[2][2] = near + far;
    P2O[2][3] = -near * far;
    P2O[3][2] = 1;

    float width = aspect * height;
    Matrix trans_ = TransToM(Vec3f(0, 0, -(far+near)/2));
    Matrix scale_ = ScaleToM(Vec3f(2.0/width, 2.0/height, 2.0/(near-far)));

    return scale_ * trans_ * P2O;
}