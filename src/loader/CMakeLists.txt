findSourceFiles(src_files)

add_library(litext_loader STATIC ${src_files})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${src_files})

target_link_libraries(litext_loader litext_global)

find_package(Boost 1.76 REQUIRED COMPONENTS filesystem program_options)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(litext_loader ${Boost_LIBRARIES})

if(NOT MSVC)
    # dep dl
    target_link_libraries(litext_loader dl)
endif()
