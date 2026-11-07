#pragma once

#include "Ray.h"

class Triangle
{
private:
    // 世界坐标系下
    Vector3f mVertices[3];      // 三个顶点
    Vector3f mNormal;           // 三角形法线

public:
    Triangle(const Vector3f &v0, const Vector3f &v1, const Vector3f &v2, const Matrix4x4 worldMatrix);
    bool Intersect(Ray ray, Intersection &isect) const;
};
