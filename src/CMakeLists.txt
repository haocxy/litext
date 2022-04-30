include(assets.cmake)
remove_useless_files_generated_for_assets()
generate_src_file_for_all_assets()

if(MSVC)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_RUNTIME ON)
endif()

set(MY_BOOST_ROOT ${CMAKE_SOURCE_DIR}/prepare/boost/install)
set(MY_BOOST_VER "1.78")


include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${MY_FREETYPE_ROOT}/include/freetype2)
include_directories(${CMAKE_SOURCE_DIR}/third/libskia)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_definitions(-DLITEXT_WORKDIR_NAME=".litext")

add_subdirectory(core)
add_subdirectory(charset)
add_subdirectory(global)
add_subdirectory(lib)
add_subdirectory(loader)
add_subdirectory(exe)

