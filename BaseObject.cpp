#include "BaseObject.h"

BaseObject::BaseObject() {}

BaseObject::BaseObject(Vec3f p):transform(p) {}

BaseObject::BaseObject(Vec3f p, Vec3f r):transform(p,r) {}

BaseObject::BaseObject(Vec3f p, Vec3f r, Vec3f s):transform(p,s,r) {}

void BaseObject::SetPosition(Vec3f p)
{
    transform.position = p;
}

void BaseObject::SetRotation(Vec3f r)
{
    transform.rotation = r;
}

void BaseObject::SetScale(Vec3f s)
{
    transform.scale = s;
}