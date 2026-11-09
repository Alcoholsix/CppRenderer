#pragma once

#include <memory>
#include "SceneObject.h"
#include "Camera.h"

class Scene
{
private:
    Camera mCamera;
    std::vector<SceneObject*> mSceneObjects;

public:
    void SetCamera(const Camera &camera) { mCamera = camera; }
    const Camera &GetCamera() const { return mCamera; }
    SceneObject *CreateSceneObject(const Vector3f &position, const Vector3f &euler, float scale);
    SceneObject *Intersect(Ray ray, Intersection &isect) const;
    ~Scene();

    // static Scene* LoadSceneFromeXML(const char *filepath, int W, int H);
    static std::unique_ptr<Scene> LoadFromXML(const std::string &filepath, int W, int H);
};
