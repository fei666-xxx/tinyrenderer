#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "BaseObject.h"
#include "utility.h"

class Camera: public BaseObject
{
protected:
    float near,far,aspect;
    Vec3f lookat;
    Vec3f x_,y_,z_;

public:
    Camera(Vec3f pos_, Vec3f lookat_, float near_, float far_, float aspect_);
    Vec3f GetXAxis();
    Vec3f GetYAxis();
    Vec3f GetZAxis();
    Vec3f GetPos();
    float GetNear();
    float GetFar();
    float GetAspect();
    Matrix ViewTrans();
    virtual Matrix ProjectTrans()=0;
};

class OrthoCamera: public Camera
{
private:
    float height;

public:
    OrthoCamera(Vec3f pos_, Vec3f lookat_, float height_, float near_=-0.1, float far_=-500, float aspect_=1);
    virtual Matrix ProjectTrans();
};

class PerspCamera: public Camera
{
private:
    float FOV;

public:
    PerspCamera(Vec3f pos_, Vec3f lookat_, float FOV_=45, float near_=-0.1, float far_=-500, float aspect_=1);
    virtual Matrix ProjectTrans();
};

#endif