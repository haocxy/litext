cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)
project(notesharp)

# 全局项目构建配置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_FIND_LIBRARY_PREFIXES "lib;")


# VS专用全局设置
if(MSVC)
    # 关闭有关源代码字符集的警告
    add_compile_options(/wd4819)
    # 关闭丢弃掉标有 [nodiscard] 的函数的返回值的编译警告
    add_compile_options(/wd4834)
    # 多处理器编译
    add_compile_options(/MP)
else()
    # 关闭丢弃掉标有 [nodiscard] 的函数的返回值的编译警告
    add_compile_options(-Wno-unused-result)
    add_link_options(-ldl)
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
add_subdirectory(third EXCLUDE_FROM_ALL)

include(release_version.cmake)
get_release_version(release_version)

set(NOTESHARP_RELEASE_VERSION ${release_version})
message(STATUS "Release Version: [${release_version}]")

if(MSVC)
    file(COPY "${CMAKE_SOURCE_DIR}/scripts/pack.bat" DESTINATION ${CMAKE_BINARY_DIR})
    include(install_for_windows.cmake)
endif()
