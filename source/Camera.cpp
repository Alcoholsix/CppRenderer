#include "Camera.h"

void Camera::Initialize(const Vector3f &p, const Vector3f &target, const Vector3f &up,
                        float fov, float n, float f, int w, int h)
{
    mPosition = p;

    // // 求观察矩阵
    // Vector3f l = glm::normalize(target - p);            // 观察方向
    // Vector3f r = glm::normalize(glm::cross(up, 1));
    // Vector3f u = glm::cross(1, r);
    // Matrix4x4 viewMatrix = glm::transpose(Matrix4x4(
    //     r.x, r.y, r.z, 0.0f,
    //     u.x, u.y, u.z, 0.0f, 
    //     l.x, l.y, l.z, 0.0f, 
    //     0.0f, 0.0f, 0.0f, 1.0f
    // ));
    // MakeTranslation(-p);

    // glm 库给出了以上的实现方法, LH 表示左手系， RH 右手系
    Matrix4x4 viewMatrix = glm::lookAtLH(p, target, up);

    // 求投影矩阵
    Matrix4x4 projectionMatrix = glm::perspectiveFovLH_ZO(fov, (float)w, (float)h, n, f);

    // 求视口矩阵
    Matrix4x4 viewportMatrix = Matrix4x4(
        w / 2.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -h / 2.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        w / 2.0f, h / 2.0f, 0.0, 1.0f
    );

    // 求以上三个联合矩阵
    Matrix4x4 combinedMatrix = viewportMatrix * projectionMatrix * viewMatrix;      // 注意矩阵乘法顺序
    Matrix4x4 invCombinedMatrix = glm::inverse(combinedMatrix);                     // 求逆矩阵

    mCombinedMatrix = combinedMatrix;
    mInvCombinedMatrix = invCombinedMatrix;
}

Ray Camera::GetRay(float x, float y) const
{
    Ray ray;
    ray.o = mPosition;

    Vector4f p(x, y, 0.0f, 1.0f);
    Vector4f worldPos = mInvCombinedMatrix * p;
    worldPos /= worldPos.w;                     // 齐次坐标除以 w 分量，得到世界坐标
    // 从相机位置指向世界坐标的方向向量
    ray.d = glm::normalize(Vector3f(worldPos) - mPosition);

    return ray;
}
