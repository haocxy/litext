cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)
project(notesharp)


# 全局项目构建配置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_FIND_LIBRARY_PREFIXES "lib;")


# 根据是否有环境变量 NOTESHARP_THIS_IS_DEBUG_MACHINE 决定
# 构建为Debug还是Release
if(DEFINED ENV{NOTESHARP_THIS_IS_DEBUG_MACHINE})
    set(CMAKE_BUILD_TYPE "Debug")
else()
    set(CMAKE_BUILD_TYPE "Release")
endif()


# Qt5支持
set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)


# VS专用全局设置
if(MSVC)
    # 关闭有关源代码字符集的警告
    add_compile_options(/wd4819)
endif()


# GNU体系全局设置
if(CMAKE_COMPILER_IS_GNUCXX)
    # 确保std::filesystem可用
    link_libraries(-lstdc++fs)
    message(STATUS "option: -lstdc++fs")
    link_libraries(-lpthread)
    message(STATUS "option: -lpthread")
endif()


add_subdirectory(src)
