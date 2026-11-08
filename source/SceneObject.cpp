#include "SceneObject.h"

bool SceneObject::Intersect(Ray ray, Intersection &isect) const
{
    bool hit = false;
    for (const auto &Primitive : mPrimitives)
    {
        if (Primitive->Intersect(ray, isect))
        {
            ray.maxt = isect.t;
            hit = true;
        }
    }

    return hit;
}

/**
 * 释放每个图形的指针
 */
SceneObject::~SceneObject()
{
    for (auto &primitive : mPrimitives)
    {
        if (primitive)
        {
            delete primitive;
            primitive = nullptr;
        }
    }
}
