#pragma once

#include "Ray.h"

class Sphere
{
private:
    float mRadius;
    Matrix4x4 mObjectToWorld;   // 对象到世界的变换矩阵
    Matrix4x4 mWorldToObject;   // 世界到对象的变换矩阵

public:
    Sphere(const Vector3f &center, float R);
    bool Intersect(Ray ray, Intersection &isect) const;        // 判断图形与一个射线是否相交

};
