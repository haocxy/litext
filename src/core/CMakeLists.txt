file(GLOB common_src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake"
)

file(GLOB win32_src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/win32/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/win32/*.cpp"
)

file(GLOB unix_src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/unix/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/unix/*.cpp"
)

set(src_files ${common_src_files})
if(MSVC)
    set(src_files "${src_files};${win32_src_files}")
else()
    set(src_files "${src_files};${unix_src_files}")
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${src_files})

add_library(litext_core STATIC ${src_files})

# dep uchardet
if(MSVC)
    # 在windows平台上, 不加 _static 后缀就是动态库
    target_link_libraries(litext_core libuchardet_static)
else()
    # 在linux平台上, 不加 _static 后缀就是静态库
    target_link_libraries(litext_core libuchardet)
endif()

# dep sqlite3
target_link_libraries(litext_core libsqlite3_static)

# dep freetype
target_link_libraries(litext_core freetype)

# dep Boost
find_package(Boost 1.76 REQUIRED COMPONENTS filesystem)
target_link_libraries(litext_core ${Boost_LIBRARIES})


if(MSVC)
    # dep skia
    target_link_libraries(litext_core ${CMAKE_SOURCE_DIR}/third/libskia/lib/skresources.lib)
    target_link_libraries(litext_core ${CMAKE_SOURCE_DIR}/third/libskia/lib/skshaper.lib)
    target_link_libraries(litext_core ${CMAKE_SOURCE_DIR}/third/libskia/lib/particles.lib)
    target_link_libraries(litext_core ${CMAKE_SOURCE_DIR}/third/libskia/lib/pathkit.lib)
    target_link_libraries(litext_core ${CMAKE_SOURCE_DIR}/third/libskia/lib/skia.lib)

    # dep opengl
    target_link_libraries(litext_core opengl32.lib)
endif()


if(NOT MSVC)
    # dep FontConfig
    find_package(Fontconfig REQUIRED)
    target_link_libraries(litext_core ${Fontconfig_LIBRARIES})

    # dep dl
    target_link_libraries(litext_core dl)
endif()

