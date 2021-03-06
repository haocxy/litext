findSourceFiles(src_files)

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
find_library(lib_freetype freetype)
target_link_libraries(litext_core ${lib_freetype})

# dep Boost
find_package(Boost ${MY_BOOST_VER} REQUIRED COMPONENTS filesystem HINTS ${MY_BOOST_ROOT})
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

