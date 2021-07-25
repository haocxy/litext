set(root_dir ${CMAKE_CURRENT_SOURCE_DIR})



include(assets.cmake)
remove_useless_files_generated_for_assets()
generate_src_file_for_all_assets()


find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)


find_package(Boost 1.76 REQUIRED
    COMPONENTS filesystem)


include_directories(${Boost_INCLUDE_DIRS})
include_directories(${root_dir})
include_directories(${CMAKE_SOURCE_DIR})


file(GLOB_RECURSE src_files
    "${root_dir}/*.h"
    "${root_dir}/*.cpp"
    "${root_dir}/*.txt"
    "${root_dir}/*.sql"
    "${root_dir}/*.cmake"
)

add_executable(notesharp
    ${src_files}
)

if(MSVC)
    target_link_options(notesharp
        # windows平台的入口为WinMain，默认的main是带命令行的
        PRIVATE "/SUBSYSTEM:WINDOWS"
    )
endif()

source_group(TREE "${root_dir}/.." FILES ${src_files})

set(deps
    Qt5::Widgets
    libuchardet_static
    libsqlite3_static
    freetype
    ${Boost_LIBRARIES}
)


if(NOT MSVC)
    list(APPEND deps dl)
endif()

target_link_libraries(notesharp
    ${deps}
)
