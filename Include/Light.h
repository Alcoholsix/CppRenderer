#pragma once

#include "Ray.h"
#include "Renderer.h"

class Light
{
private:

public:
    // 对于一点 p，求它的 L(p)
    virtual Color GetRadiance(const Vector3f &p, Vector3f &sourcePos) const = 0;

};

/* 平行光 */
class DirectionalLight : public Light
{
private:
    Vector3f mDirection;        // 光线方向，单位向量
    Color mRadiance;            // 光照强度

public:
    DirectionalLight(const Vector3f &direction, const Color &radiance)
        : mDirection(glm::normalize(direction)), mRadiance(radiance) { }

    Color GetRadiance(const Vector3f &p, Vector3f &sourcePos) const override;
};

/* 点光源 */
class PointLight : public Light
{
private:
    Vector3f mPosition;     // 光源位置
    Color mIntensity;       // 光照强度
    Vector3f mAttenuations; // 衰减系数，分别表示 A, B, C

public:
    PointLight(const Vector3f &position, const Color &intensity, const Vector3f &attenuations)
        : mPosition(position), mIntensity(intensity), mAttenuations(attenuations) { }
    Color GetRadiance(const Vector3f &p, Vector3f &sourcePos) const override;
};

/* 聚光灯 */
class SpotLight : public Light
{
private:
    Vector3f mDirection;        // 光线方向
    Vector3f mPosition;         // 光源位置
    Color mIntensity;           // 光照强度
    float mCosInnerAngle;       // 内角度, alpha
    float mCosOuterAngle;       // 外角度, beta
    Vector3f mAttenuations;     // 衰减系数，分别为 A，B，C

public:
    SpotLight(const Vector3f &position, const Vector3f &direction, const Color &intensity,
                float innerAngle, float outerAngle, const Vector3f &attenuations)
        : mPosition(position), mDirection(glm::normalize(direction)), mIntensity(intensity),
        mCosInnerAngle(glm::cos(innerAngle)), mCosOuterAngle(glm::cos(outerAngle)), mAttenuations(attenuations)
    {
    }

    virtual Color GetRadiance(const Vector3f &p, Vector3f &sourcePos) const override;
};
