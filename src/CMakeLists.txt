include(assets.cmake)
remove_useless_files_generated_for_assets()
generate_src_file_for_all_assets()

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third/libskia)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

if(MSVC)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_STATIC_RUNTIME ON)
endif()

add_definitions(-DLITEXT_WORKDIR_NAME=".litext")

add_subdirectory(core)
add_subdirectory(global)
add_subdirectory(lib)
add_subdirectory(loader)
add_subdirectory(exe)

