find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)


set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.76 REQUIRED
    COMPONENTS filesystem program_options)
include_directories(${Boost_INCLUDE_DIRS})

include_directories($ENV{SKIA_ROOT})

if(NOT MSVC)
    find_package(Fontconfig REQUIRED)
    include_directories(${Fontconfig_INCLUDE_DIRS})
endif()

file(GLOB_RECURSE src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.sql"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake"
)

add_library(libnotesharp STATIC ${src_files})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${src_files})


set_target_properties(Qt5::Core PROPERTIES
    MAP_IMPORTED_CONFIG_DEBUG "RELEASE"
)
set_target_properties(Qt5::Gui PROPERTIES
    MAP_IMPORTED_CONFIG_DEBUG "RELEASE"
)
set_target_properties(Qt5::Widgets PROPERTIES
    MAP_IMPORTED_CONFIG_DEBUG "RELEASE"
)

target_link_libraries(libnotesharp Qt5::Widgets)
target_link_libraries(libnotesharp libuchardet_static)
target_link_libraries(libnotesharp libsqlite3_static)
target_link_libraries(libnotesharp freetype)
target_link_libraries(libnotesharp ${Boost_LIBRARIES})

if(MSVC)
    target_link_libraries(libnotesharp $ENV{SKIA_ROOT}/lib/skresources.lib)
    target_link_libraries(libnotesharp $ENV{SKIA_ROOT}/lib/skshaper.lib)
    target_link_libraries(libnotesharp $ENV{SKIA_ROOT}/lib/particles.lib)
    target_link_libraries(libnotesharp $ENV{SKIA_ROOT}/lib/pathkit.lib)
    target_link_libraries(libnotesharp $ENV{SKIA_ROOT}/lib/skia.lib)
    target_link_libraries(libnotesharp opengl32.lib)
else()
    target_link_libraries(libnotesharp dl)
    target_link_libraries(libnotesharp ${Fontconfig_LIBRARIES})
endif()
