#ifndef __BASEOBJECT_H__
#define __BASEOBJECT_H__

#include "geometry.h"

class BaseObject
{

public:
    Transform transform;

public:
    BaseObject();
    BaseObject(Vec3f p);
    BaseObject(Vec3f p, Vec3f r);
    BaseObject(Vec3f p, Vec3f r, Vec3f s);
    void SetPosition(Vec3f p);
    void SetRotation(Vec3f r);
    void SetScale(Vec3f s);
};

#endif