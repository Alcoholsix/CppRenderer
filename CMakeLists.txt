# 指定 cmake 最低版本, 项目名称
cmake_minimum_required(VERSION 3.20)
project(CppRenderer LANGUAGES CXX)

# 指定 c++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 项目本身的源文件
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/source/*.cpp" 
    "${CMAKE_SOURCE_DIR}/source/*.h" 
    "${CMAKE_SOURCE_DIR}/source/*.hpp" 
)

# 创建可执行文件
add_executable(renderer ${APP_SOURCES})
target_include_directories(renderer PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# 头文件附加目录：minifb/include & glm
target_include_directories(renderer PRIVATE
    "${CMAKE_SOURCE_DIR}/minifb/include"
    "${CMAKE_SOURCE_DIR}/glm"
)

# 添加 minifb 到构建项目：
add_subdirectory(minifb)

# 连接库
target_link_libraries(renderer PRIVATE minifb)
if (WIN32)
target_link_libraries(renderer PRIVATE
    minifb
    opengl32
    winmm
    kernel32
    user32
    gdi32
    winspool
    shell32
    ole32
    oleaut32
    uuid
    comdlg32
    advapi32)
endif()
