set(root_dir ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_AUTOMOC ON)

include(assets.cmake)
remove_useless_files_generated_for_assets()
generate_src_file_for_all_assets()

find_package(Boost 1.76 REQUIRED
    COMPONENTS filesystem)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${root_dir})
include_directories(${CMAKE_SOURCE_DIR})
include_directories($ENV{LIBFONTCONFIG_INCLUDE})

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

if(MSVC)
    list(APPEND deps $ENV{LIBCAIRO_LIB})
    list(APPEND deps $ENV{LIBFONTCONFIG_LIB})
else()
    list(APPEND deps dl)
    list(APPEND deps cairo)
    list(APPEND deps png)
    list(APPEND deps z)
    list(APPEND deps pixman-1)
    list(APPEND deps fontconfig)
endif()

target_link_libraries(notesharp
    ${deps}
)
