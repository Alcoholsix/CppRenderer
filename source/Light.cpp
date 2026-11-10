#include <Light.h>

Color DirectionalLight::GetRadiance(const Vector3f &p, Vector3f &sourcePos) const
{
    // 光源应该在无限远处
    sourcePos = p - mDirection * 100000.0f;
    return mRadiance;
}

Color PointLight::GetRadiance(const Vector3f &p, Vector3f &sourcePos) const
{
    sourcePos = mPosition;
    float R = glm::length(p - mPosition);
    float attenuation = 1.0f / (mAttenuations.z + mAttenuations.y * R + mAttenuations.x * R * R);
    return attenuation * mIntensity;
}

Color SpotLight::GetRadiance(const Vector3f &p, Vector3f &sourcePos) const
{
    sourcePos = mPosition;
    // 距离衰减，k1
    float R = glm::length(p - mPosition);
    float k1 = 1.0f / (mAttenuations.z + mAttenuations.y * R + mAttenuations.x * R * R);

    // 角度衰减，k2
    Vector3f L = glm::normalize(p - mPosition);
    float cosTheta = glm::dot(L, mDirection);
    float k2 = (cosTheta - mCosOuterAngle) / (mCosInnerAngle - mCosOuterAngle);

    return mIntensity * k1 * glm::clamp(k2, 0.0f, 1.0f);
}
