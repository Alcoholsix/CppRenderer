#pragma once
#include <atomic>
#include <vector>
#include <memory>
#include <string>

#include "Common.h"
#include "Camera.h"
#include "Sphere.h"
#include "Disk.h"
#include "Triangle.h"
#include "Scene.h"

using Color = Vector3f;

class Renderer
{
public:
    Renderer(int w, int h, const std::string path);
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

    // Scene *mScene = nullptr;
    std::unique_ptr<Scene> mScene;
    std::string mFilePath;          // 读取 XML 文件的路径
};
