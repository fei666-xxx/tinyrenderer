#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "BaseObject.h"

class Light: public BaseObject
{
public:
    Vec3f LightDir;
public:
    Light();
    Light(Vec3f p);
    Light(Vec3f p, Vec3f r);
    Light(Vec3f p, Vec3f r, Vec3f s);
    void SetDir(Vec3f l);
};

#endif