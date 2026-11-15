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
    // 销毁创建的物体指针
    for (SceneObject *pSceneObject : mSceneObjects)
        delete pSceneObject;
    
    // 销毁创建的灯光指针
    for (Light *pLight : mLights)
        delete pLight;
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
