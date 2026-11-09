#include <MiniFB.h>
#include <thread>
#include <vector>

#include "Renderer.h"

/**
 * 相机和物体的调用方法
 */
Renderer::Renderer(int w, int h)
    : mViewportWidth(w), mViewportHeight(h)
{
    mCurrentPixelIndex = 0;
    mScene = new Scene();
    
    // 设置摄像机
    Camera camera;
    camera.Initialize(
        Vector3f(0.0f, 0.0f, 0.0f),     // 相机位置
        Vector3f(0.0f, 0.0f, 1.0f),     // 目标位置
        Vector3f(0.0f, 1.0f, 0.0f),     // 上向量   
        glm::radians(60.0f),            // FOV
        0.1f,                           // 近裁剪面
        1000.0f,                        // 远裁剪面
        w, h
    );
    mScene->SetCamera(camera);

    // 给场景添加物体
    SceneObject *pSceneObject = mScene->CreateSceneObject(Vector3f(0, 0, 5), Vector3f(0, 0, 0), 2.0f);
    pSceneObject->CreatePrimitive<Triangle>(Vector3f(-1, -1, 0), Vector3f(1, -1, 0), Vector3f(1, 1, 0));
    pSceneObject->CreatePrimitive<Triangle>(Vector3f(-1, -1, 0), Vector3f(1, 1, 0), Vector3f(-1, 1, 0));

    SceneObject *pSceneObject2 = mScene->CreateSceneObject(Vector3f(0, 0, 2), Vector3f(0, 0, 0), 1.0f);
    pSceneObject2->CreatePrimitive<Sphere>(0.5f);
}

/**
 * Renderer 的析构函数，函数结束后会自动调用
 * 这里用来释放构建完图形后的指针
 */
Renderer::~Renderer()
{
    if (mScene)
    {
        delete mScene;
        mScene = nullptr;
    }
}

/**
 * 窗口绘制主程序
 */
void Renderer::Run()
{
    struct mfb_window *window = mfb_open_ex("cppRenderer", mViewportWidth, mViewportHeight, MFB_WF_RESIZABLE);
    if (window == NULL)
        return;

    // 屏幕上每个像素点的颜色用32位表示 格式：0xAARRGGBB(Alpha, Red, Green, Blue)
    mBuffer = (uint32_t*)malloc(mViewportWidth * mViewportHeight * 4);

    // 创建渲染线程
    // 多线程，渲染线程开启后，不阻塞主线程执行
    std::thread renderThread(&Renderer::RunRenderThread, this);
    renderThread.detach();                                          // 主线程结束后，子线程也结束

    int numThreads = std::thread::hardware_concurrency() - 6;           // 返回计算机核心数 16 - 6
    std::cout << "Work Hardware concurrency: " << numThreads << std::endl;
    std::vector<std::thread> renderThreads(numThreads);
    for (int i = 0; i < numThreads; i++)                            // 给每个核心创建一个线程
    {
        renderThreads[i] = std::thread(&Renderer::RunRenderThread, this);
        renderThreads[i].detach();
    }

    // Present
    mfb_update_state state;
    do {
        // TODO: add some fancy rendering to the buffer of size height * width

        state = mfb_update_ex(window, mBuffer, mViewportWidth, mViewportHeight);

        if (state != MFB_STATE_OK)
            break;

    } while(mfb_wait_sync(window));

    free(mBuffer);
    mBuffer = NULL;
    window = NULL;
}

/**
 * ** 核心方法 **
 * 像素渲染算法，实现 SSAA 超级采样算法
 */
Color Renderer::RenderPixel(int x, int y)
{
    // SSAA 超级采样算法，每个像素点取多次采样均值
    static const int N = 32;
    Color resultColor (0, 0, 0);

    for (int i = 0; i < N; i++)
    {
        // (x, y) - (x+1, y+1) 范围内，随机采样一个点
        float dx = x + glm::linearRand(0.0f, 1.0f);
        float dy = y + glm::linearRand(0.0f, 1.0f);

        Color color = RenderSubPixel(dx, dy);
        resultColor += color / (float)N;
    }

    return resultColor;
}

/**
 * 计算当前颜色的方法
 */
Color Renderer::RenderSubPixel(float x, float y)
{
    Ray ray = mScene->GetCamera().GetRay(x, y);
    Intersection isect;
    Color color(0, 0, 0);

    color.r = (float)x / mViewportWidth;
    color.g = 0.0f;
    color.b = 1.0f - (float)y / mViewportHeight;

    // 屏幕上最先绘制靠近屏幕的内容，即返回t值最小
    /* *****************************************************************************************
    bool bHit = false;
    for (const auto &Primitive : mPrimitives)
    {
        if (Primitive->Intersect(ray, isect))
        {
            ray.maxt = isect.t;
            bHit = true;
        }
    }
    // 只有射线碰撞到，才返回颜色值
    if (bHit)
        color = isect.normal * 0.5f + 0.5f;

    return color;
    ***************************************************************************************** */

    if (mScene->Intersect(ray, isect))
    {
        color = isect.normal * 0.5f + 0.5f;
    }

    return color;
}

/**
 * 渲染线程的入口函数
 */
void Renderer::RunRenderThread()
{
    // 读取当前屏幕的下一个像素， 填充buffer
    while (true)
    {
        int pixelIndex = mCurrentPixelIndex.fetch_add(1);           // 当前值加一，返回加前的数据
        if (pixelIndex >= mViewportWidth * mViewportHeight)         // 超出屏幕范围退出
            break;

        int x = pixelIndex % mViewportWidth;
        int y = pixelIndex / mViewportWidth;

        Color color = RenderPixel(x, y);
        uint8_t r = glm::clamp((uint32_t)std::round(color.r * 255.0f), 0u, 255u);
        uint8_t g = glm::clamp((uint32_t)std::round(color.g * 255.0f), 0u, 255u);
        uint8_t b = glm::clamp((uint32_t)std::round(color.b * 255.0f), 0u, 255u);
        uint8_t a = 255;
        mBuffer[y * mViewportWidth + x] = (a << 24) | (r << 16) | (g << 8) | b;
    }
}
