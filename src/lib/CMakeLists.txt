set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.sql"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${src_files})

add_library(litext_body SHARED ${src_files})

target_link_libraries(litext_body litext_global)

# dep Qt5
find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)

set_target_properties(Qt5::Core PROPERTIES MAP_IMPORTED_CONFIG_DEBUG "RELEASE")
set_target_properties(Qt5::Gui PROPERTIES MAP_IMPORTED_CONFIG_DEBUG "RELEASE")
set_target_properties(Qt5::Widgets PROPERTIES MAP_IMPORTED_CONFIG_DEBUG "RELEASE")
set_target_properties(Qt5::Network PROPERTIES MAP_IMPORTED_CONFIG_DEBUG "RELEASE")
target_link_libraries(litext_core Qt5::Widgets Qt5::Network)

