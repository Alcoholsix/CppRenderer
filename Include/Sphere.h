#pragma once

#include "Primitive.h"

class Sphere : public Primitive
{
private:
    float mRadius;

public:
    Sphere(SceneObject *pSceneObject, float R);
    virtual bool Intersect(Ray ray, Intersection &isect) const override;       // 判断图形与一个射线是否相交
};
