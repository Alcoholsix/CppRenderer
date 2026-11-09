#pragma once

#include <vector>
#include "Ray.h"
#include "Primitive.h"

class SceneObject
{
private:
    Matrix4x4 mObjectToWorld;
    Matrix4x4 mWorldToObject;

    std::vector<Primitive*> mPrimitives;    // 场景对象包含的所有基本图元

public:
    Matrix4x4 GetObjectToWorld() const {return mObjectToWorld;}
    Matrix4x4 GetWorldToObject() const {return mWorldToObject;}

    SceneObject(const Vector3f &position, const Vector3f &euler, float scale)
    {
        mObjectToWorld = MakeWorldTransform(position, euler, scale);
        mWorldToObject = glm::inverse(mObjectToWorld);
    }

    bool Intersect(Ray ray, Intersection &isect) const;

    void AddPrimitive(Primitive *primitive)
    {
        mPrimitives.push_back(primitive);
    }

    // 教程说这种写法看起来更高级，显得牛逼 :)
    template<typename T, typename...Args>       // 参数 T 和可变参数列表 Args
    T *CreatePrimitive(Args&&... args)
    {
        T *Primitive = new T(this, std::forward<Args>(args)...);
        mPrimitives.push_back(Primitive);

        return Primitive;
    }

    virtual ~SceneObject();
};
