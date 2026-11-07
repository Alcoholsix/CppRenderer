# 项目名称
    **c++ 从零打造渲染器**

# 项目简介 #
2026年11月6日，跟着B站up主 “发财学长” 的 “c++从零打造渲染器” 项目进行练习。

## 下载安装内容 ##
1. 下载和安装 CMake
    - 网址：https://cmake.org
    - 注意事项：勾选两个add选项
2. 下载 GLM 库（数学库）
    - 网址：https://github.com/g-truc/glm
3. 下载 minifb 库（显示简单的窗口）
    - 网址：https://github.com/emoon/minifb

### Code demo ###
```cpp
#include <iostream>

int main()
{
    std::cout << "Hello world!" << endl;
}
```

## minifb 测试代码 ##
``` cpp
int main() {
    struct mfb_window *window = mfb_open_ex("my display", 800, 600, MFB_WF_RESIZABLE);
    if (window == NULL)
        return 0;

    uint32_t *buffer = malloc(800 * 600 * 4);

    mfb_update_state state;
    do {
        // TODO: add some fancy rendering to the buffer of size 800 * 600

        state = mfb_update_ex(window, buffer, 800, 600);

        if (state != MFB_STATE_OK)
            break;

    } while(mfb_wait_sync(window));

    free(buffer);
    buffer = NULL;
    window = NULL;

    return 0;
}
```
