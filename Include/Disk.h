#pragma once

#include "primitive.h"

class Disk : public Primitive
{
public:
    Disk(const Vector3f &center, const Vector3f &euler, float radius);
    virtual bool Intersect(Ray ray, Intersection &isect) const override;

private:
    float mRadius;
    Matrix4x4 mObjectToWorld;
    Matrix4x4 mWorldToObject;
};
