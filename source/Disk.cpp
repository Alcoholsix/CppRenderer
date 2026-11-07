#include "Disk.h"

Disk::Disk(const Vector3f &center, const Vector3f &euler, float radius)
    : mRadius(radius)
{
    mObjectToWorld = MakeWorldTransform(center, euler, 1.0f);
    mWorldToObject = glm::inverse(mObjectToWorld);
}

bool Disk::Intersect(Ray ray, Intersection &isect) const
{
    // ray 转到圆盘的局部空间
    Ray r = mWorldToObject * ray;

    // t
    if (fabs(r.d.z) < 1e-6f)        return false;   // z 值为0，平行，无焦点
    float t = -r.o.z / r.d.z;                       // 圆盘所在平面的 z = 0
    
    // 交点不在射线的有效位置
    if(t < r.mint || t > r.maxt)    return false;
    Vector3f p = r.o + t * r.d;                     // 焦点位置（局部空间）
    // 判断焦点是否在圆盘内： |p| <= radius
    if (glm::dot(p, p) > mRadius * mRadius)
        return false;                               // 焦点在圆盘外

    isect.position = Vector3f(mObjectToWorld * Vector4f(p, 1.0f));  // 交点位置（世界空间）
    isect.normal = glm::normalize(Vector3f(mObjectToWorld * Vector4f(0, 0, 1, 0)));
    isect.t = t;

    return true;
}
