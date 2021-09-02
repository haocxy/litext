include(assets.cmake)
remove_useless_files_generated_for_assets()
generate_src_file_for_all_assets()

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/third/libskia)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_subdirectory(lib)
add_subdirectory(exe)

