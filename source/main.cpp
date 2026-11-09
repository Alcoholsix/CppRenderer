#include "Renderer.h"

const short viewWidth = 1200;
const short viewHeight = 800;

int main()
{
    // 后续应改写为 main 函数形参赋值，采用命令行赋值
    Renderer renderer(viewWidth, viewHeight, "test/TestScene2.xml");
    renderer.Run();

    return 0;
}
