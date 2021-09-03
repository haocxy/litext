file(GLOB_RECURSE src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake"
)

add_library(libnotesharp_loader STATIC ${src_files})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${src_files})

find_package(Boost 1.76 REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(libnotesharp_loader ${Boost_LIBRARIES})
