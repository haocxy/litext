find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)

find_package(Boost 1.76 REQUIRED
    COMPONENTS filesystem program_options)
include_directories(${Boost_INCLUDE_DIRS})

if(MSVC)
    # Debug配置启用AddressSanitizer技术
    #add_compile_options($<$<CONFIG:Debug>:/fsanitize=address>)
endif()

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

set(deps
    Qt5::Widgets
    libuchardet_static
    libsqlite3_static
    freetype
    ${Boost_LIBRARIES}
)

if(NOT MSVC)
    list(APPEND deps dl)
    list(APPEND deps ${Fontconfig_LIBRARIES})
endif()

target_link_libraries(libnotesharp ${deps})
