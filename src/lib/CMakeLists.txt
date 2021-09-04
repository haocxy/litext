set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.sql"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${src_files})

add_library(liblitext SHARED ${src_files})


# dep uchardet
if(MSVC)
    # 在windows平台上, 不加 _static 后缀就是动态库
    target_link_libraries(liblitext libuchardet_static)
else()
    # 在linux平台上, 不加 _static 后缀就是静态库
    target_link_libraries(liblitext libuchardet)
endif()

# dep sqlite3
target_link_libraries(liblitext libsqlite3_static)

# dep freetype
target_link_libraries(liblitext freetype)

# dep Qt5
find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)

set_target_properties(Qt5::Core PROPERTIES MAP_IMPORTED_CONFIG_DEBUG "RELEASE")
set_target_properties(Qt5::Gui PROPERTIES MAP_IMPORTED_CONFIG_DEBUG "RELEASE")
set_target_properties(Qt5::Widgets PROPERTIES MAP_IMPORTED_CONFIG_DEBUG "RELEASE")
set_target_properties(Qt5::Network PROPERTIES MAP_IMPORTED_CONFIG_DEBUG "RELEASE")
target_link_libraries(liblitext Qt5::Widgets Qt5::Network)

# dep Boost
find_package(Boost 1.76 REQUIRED COMPONENTS filesystem)
target_link_libraries(liblitext ${Boost_LIBRARIES})


if(MSVC)
    # dep skia
    target_link_libraries(liblitext ${CMAKE_SOURCE_DIR}/third/libskia/lib/skresources.lib)
    target_link_libraries(liblitext ${CMAKE_SOURCE_DIR}/third/libskia/lib/skshaper.lib)
    target_link_libraries(liblitext ${CMAKE_SOURCE_DIR}/third/libskia/lib/particles.lib)
    target_link_libraries(liblitext ${CMAKE_SOURCE_DIR}/third/libskia/lib/pathkit.lib)
    target_link_libraries(liblitext ${CMAKE_SOURCE_DIR}/third/libskia/lib/skia.lib)

    # dep opengl
    target_link_libraries(liblitext opengl32.lib)
endif()


if(NOT MSVC)
    # dep FontConfig
    find_package(Fontconfig REQUIRED)
    target_link_libraries(liblitext ${Fontconfig_LIBRARIES})

    # dep dl
    target_link_libraries(liblitext dl)
endif()
