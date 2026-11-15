#pragma once

#include <memory>
#include "SceneObject.h"
#include "Camera.h"
#include "Light.h"

class Scene
{
private:
    Camera mCamera;
    std::vector<SceneObject*> mSceneObjects;
    std::vector<Light*> mLights;                // 存场景中的所有光源

public:
    void SetCamera(const Camera &camera) { mCamera = camera; }
    const Camera &GetCamera() const { return mCamera; }
    SceneObject *CreateSceneObject(const Vector3f &position, const Vector3f &euler, float scale);
    SceneObject *Intersect(Ray ray, Intersection &isect) const;
    ~Scene();

    // static Scene* LoadSceneFromeXML(const char *filepath, int W, int H);
    static std::unique_ptr<Scene> LoadFromXML(const std::string &filepath, int W, int H);

    template<typename T, typename... Args>        // 因光源的参数不同，构建一个可变参数列表
    Light* CreateLight(Args && ...args)
    {
        Light *light = new T(std::forward<Args>(args)...);
        mLights.push_back(light);
        return light;
    }

    const std::vector<Light*>& GetLights() const { return mLights; }
};
