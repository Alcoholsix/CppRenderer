#pragma once
#include "Common.h"
#include <atomic>

using Color = Vector3f;

class Renderer
{
public:
    Renderer(int w, int h);
    void Run();

private:
    Color RenderPixel(int x, int y);
    void RunRenderThread();

    int mViewportWidth = 800;
    int mViewportHeight = 600;
    uint32_t* mBuffer = nullptr;

    // 原子类型，像素指针
    std::atomic<int> mCurrentPixelIndex = 0;
};
