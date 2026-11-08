#pragma once

#include "Ray.h"

class SceneObject;

class Primitive
{
protected:
    SceneObject *m_pSceneObject = nullptr;

public:
    virtual bool Intersect(Ray ray, Intersection &isect) const = 0;
    Primitive(SceneObject *pSceneObject) : m_pSceneObject(pSceneObject)
    {

    }
};
