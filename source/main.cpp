#include "Renderer.h"

const short viewWidth = 1200;
const short viewHeight = 800;

int main()
{
    Renderer renderer(viewWidth, viewHeight);
    renderer.Run();

    return 0;
}
