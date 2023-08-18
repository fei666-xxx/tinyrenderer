#include "light.h"

Light::Light() {}

Light::Light(Vec3f p):BaseObject(p) {}

Light::Light(Vec3f p, Vec3f r):BaseObject(p,r) {}

Light::Light(Vec3f p, Vec3f r, Vec3f s):BaseObject(p,s,r) {}

void Light::SetDir(Vec3f l)
{
    this->LightDir = l;
}