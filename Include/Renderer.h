#pragma once
#include <atomic>
#include <vector>

#include "Common.h"
#include "Camera.h"
#include "Sphere.h"
#include "Disk.h"
#include "Triangle.h"

using Color = Vector3f;

class Renderer
{
public:
    Renderer(int w, int h);
    virtual ~Renderer();

    void Run();

private:
    Color RenderPixel(int x, int y);
    Color RenderSubPixel(float x, float y);
    void RunRenderThread();

    int mViewportWidth = 800;
    int mViewportHeight = 600;
    uint32_t* mBuffer = nullptr;

    // 原子类型，像素指针
    std::atomic<int> mCurrentPixelIndex = 0;

    Camera mCamera;

    Sphere* mSphere;
    Disk* mDisk;
    Triangle* mTriangle;

    // std::vector<Primitive*> mPrimitives;        // 场景中的所有集合体
    SceneObject *mTestSceneObject = nullptr;
};
