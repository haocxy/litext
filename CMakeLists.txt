cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)
project(litext)

# 自己实现的 CMake 工具
include(CMake/find_source_files.cmake)

# 全局项目构建配置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_FIND_LIBRARY_PREFIXES "lib;")

# VS专用全局设置
if(MSVC)
    # 关闭有关源代码字符集的警告
    add_compile_options(/wd4819)
    # 关闭丢弃掉标有 [nodiscard] 的函数的返回值的编译警告
    add_compile_options(/wd4834)
    # 多处理器编译
    add_compile_options(/MP)
    # Debug配置启用AddressSanitizer技术
    #add_compile_options($<$<CONFIG:Debug>:/fsanitize=address>)
    # 使用静态运行库
    add_compile_options(/MT)
    # 定义支持的最低windows版本(本项暂时定位面向win10及以上的windows版本)
    add_definitions(/D_WIN32_WINNT=0x0A00)
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


# 更新Qt翻译文件(.ts)和发布文件(.qm文件)
execute_process(
    COMMAND "lupdate" "src/lib/qt" "-ts" "trans/trans.zh_CN.ts"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
execute_process(
    COMMAND "lrelease" "trans/trans.zh_CN.ts" "-qm" "trans/trans.zh_CN.qm"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_subdirectory(src)
add_subdirectory(third EXCLUDE_FROM_ALL)


set_target_properties(
    litext_body litext
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out$<CONFIG>"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out$<CONFIG>"
)

if(MSVC)
    set_target_properties(
        litextcmd
        PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out$<CONFIG>"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out$<CONFIG>"
    )
endif()

include(release_version.cmake)
get_release_version(release_version)

set(LITEXT_RELEASE_VERSION ${release_version})
message(STATUS "Release Version: [${release_version}]")

if(MSVC)
    file(COPY "${CMAKE_SOURCE_DIR}/tools/win32/pack.bat" DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY "${CMAKE_SOURCE_DIR}/tools/win32/repack.bat" DESTINATION ${CMAKE_BINARY_DIR})
    include(install_for_windows.cmake)
endif()
