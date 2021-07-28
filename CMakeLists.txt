cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)
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


# VS专用全局设置
if(MSVC)
    # 关闭有关源代码字符集的警告
    add_compile_options(/wd4819)
    # 关闭丢弃掉标有 [nodiscard] 的函数的返回值的编译警告
    add_compile_options(/wd4834)
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


file(GLOB_RECURSE asset_files "assets/*")
install(FILES ${asset_files} DESTINATION assets)

install(TARGETS notesharp RUNTIME DESTINATION bin CONFIGURATIONS Release)

find_file(dlib_qt5core "Qt5Core.dll")
install(FILES ${dlib_qt5core} DESTINATION bin)

find_file(dlib_qt5gui "Qt5Gui.dll")
install(FILES ${dlib_qt5gui} DESTINATION bin)

find_file(dlib_qt5widgets "Qt5Widgets.dll")
install(FILES ${dlib_qt5widgets} DESTINATION bin)

find_file(dlib_zlibd "zlibd.dll")
install(FILES ${dlib_zlibd} DESTINATION bin)

find_file(dlib_png "libpng16d.dll")
install(FILES ${dlib_png} DESTINATION bin)

find_file(dlib_msvcp140 "MSVCP140.dll")
install(FILES ${dlib_msvcp140} DESTINATION bin)

find_file(dlib_vcruntime140 "VCRUNTIME140.dll")
install(FILES ${dlib_vcruntime140} DESTINATION bin)

find_file(dlib_vcruntime140_1 "VCRUNTIME140_1.dll")
install(FILES ${dlib_vcruntime140_1} DESTINATION bin)

find_file(dlib_vcruntime140d "VCRUNTIME140D.dll")
install(FILES ${dlib_vcruntime140d} DESTINATION bin)

find_file(dlib_ucrtbased "ucrtbased.dll")
install(FILES ${dlib_ucrtbased} DESTINATION bin)

find_file(dlib_qtplugin_platforms_windows "qwindows.dll" PATH_SUFFIXES plugins/platforms)
install(FILES ${dlib_qtplugin_platforms_windows} DESTINATION bin/platforms)

find_file(dlib_qtplugin_styles_vistastyle "qwindowsvistastyle.dll" PATH_SUFFIXES plugins/styles)
install(FILES ${dlib_qtplugin_styles_vistastyle} DESTINATION bin/styles)

set(CPACK_GENERATOR ZIP)
set(CPACK_PACKAGE_NAME "NoteSharp")
include(CPack)


