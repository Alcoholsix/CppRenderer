#include "Scene.h"

SceneObject *Scene::CreateSceneObject(const Vector3f &position, const Vector3f &euler, float scale)
{
    SceneObject *pSceneObject = new SceneObject(position, euler, scale);
    mSceneObjects.push_back(pSceneObject);

    return pSceneObject;
}

/**
 * Scene 的析构，负责销毁 Scene 中创建的指针
 */
Scene::~Scene()
{
    for (SceneObject *pSceneObject : mSceneObjects)
        delete pSceneObject;
}

SceneObject *Scene::Intersect(Ray ray, Intersection &isect) const
{
    SceneObject *pHitObject = nullptr;

    for (const auto pSceneobject : mSceneObjects)
    {
        if (pSceneobject->Intersect(ray, isect))
        {
            ray.maxt = isect.t;
            pHitObject = pSceneobject;
        }
    }
    return pHitObject;
}
