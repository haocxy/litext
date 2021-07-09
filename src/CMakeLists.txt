set(root_dir ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${root_dir})
include_directories(${CMAKE_SOURCE_DIR}/third/uchardet-0.0.7/src)

file(GLOB_RECURSE src_files
    "${root_dir}/*.h"
    "${root_dir}/*.cpp"
)

add_executable(notesharp
    ${src_files}
)

source_group(TREE "${root_dir}/.." FILES ${src_files})

target_link_libraries(notesharp
    Qt5::Widgets
    libuchardet)
