#include <MiniFB.h>
#include <thread>
#include <vector>

#include "Renderer.h"

Renderer::Renderer(int w, int h)
{
    mViewportWidth = w;
    mViewportHeight = h;
}

void Renderer::Run()
{
    struct mfb_window *window = mfb_open_ex("my display", mViewportWidth, mViewportHeight, MFB_WF_RESIZABLE);
    if (window == NULL)
        return;

    // 屏幕上每个像素点的颜色用32位表示 格式：0xAARRGGBB(Alpha, Red, Green, Blue)
    mBuffer = (uint32_t*)malloc(mViewportWidth * mViewportHeight * 4);

    // 创建渲染线程
    // 多线程，渲染线程开启后，不阻塞主线程执行
    std::thread renderThread(&Renderer::RunRenderThread, this);
    renderThread.detach();                                          // 主线程结束后，子线程也结束

    int numThreads = std::thread::hardware_concurrency();           // 返回计算机核心数 我的是 16 核
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

Color Renderer::RenderPixel(int x, int y)
{
    Color color;
    color.r = (float)x / mViewportWidth;
    color.g = 0.0f;
    color.b = 1.0f - (float)y / mViewportHeight;

    // 模拟像素绘制时间
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    return color;
}


// 渲染线程的入口函数
void Renderer::RunRenderThread()
{
    // 填充 buffer：
    for(int y = 0; y < mViewportHeight; y++)
    {
        for (int x = 0; x < mViewportWidth; x++)
        {
            Color color = RenderPixel(x, y);
            uint8_t r = glm::clamp((uint32_t)std::round(color.r * 255.0f), 0u, 255u);
            uint8_t g = glm::clamp((uint32_t)std::round(color.g * 255.0f), 0u, 255u);
            uint8_t b = glm::clamp((uint32_t)std::round(color.b * 255.0f), 0u, 255u);
            uint8_t a = 255;
            mBuffer[y * mViewportWidth + x] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }

    // 读取当前屏幕的下一个像素
}
