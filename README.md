# 项目名称
    C++ 从零打造渲染器

## 项目简介 ##
2026年11月6日，跟着B站up主 “发财学长” 的 “c++从零打造渲染器” 系列教程进行 c++ 项目编程学习。<br> <!-- 这是换行标致 -->

**视频链接：**[点击进入up主“发财学长”的B站教程](https://www.bilibili.com/video/BV1BMEs6UEp7?spm_id_from=333.788.videopod.sections&vd_source=e172ca534beb0a9ac941de57d86499ef)

这个项目包含了三个开源库。源文件夹比较庞杂，你可以使用我的精简过的 数学库 ， 显示窗口库 和 XML解析库，删除了开发中用不到的冗余内容，精简但不会对使用造成任何影响。但是，依然建议光顾原项目仓库，**仓库地址**见下面的**下载安装内容**。

## 下载安装内容 ##
1. 下载和安装 CMake
    - 网址：https://cmake.org
2. 下载 GLM 库（数学库）
    - 网址：https://github.com/g-truc/glm
3. 下载 minifb 库（显示简单的窗口）
    - 网址：https://github.com/emoon/minifb
4. 下载 TinyXML-2 库（轻量XML文件解析库）
    - 网址：https://github.com/leethomason/tinyxml2

**在此之前，你得确保你已经配置好了C/C++开发环境。**

### git 常用命令 ###
```powershell
###############
# git 项目创建 #
###############

# 初始化 git 仓库
git init

# 创建 git 忽略规则文件
New-Item .gitignore -ItemType File

###############
# git 项目提交 #
###############

# 将更新加入缓存区
git add -A

# 创建一个本地版本记录
git commit -m "本次提交记录名称"

# 提交到 GitHub 仓库
git push
```

## minifb 测试代码 ##
``` cpp
int main() {
    struct mfb_window *window = mfb_open_ex("my display", 800, 600, MFB_WF_RESIZABLE);
    if (window == NULL)
        return 0;

    uint32_t *buffer = (uint32_t*)malloc(800 * 600 * 4);

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
