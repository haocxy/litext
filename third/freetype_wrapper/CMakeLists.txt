set(BUILD_SHARED_LIBS OFF)

if(NOT EXISTS freetype)
    execute_process(
        COMMAND git clone https://gitee.com/haocxy/freetype.git
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    execute_process(
        COMMAND git checkout VER-2-10-4
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/freetype
    )
endif()

if(NOT MSVC)
    add_compile_options(-fPIC)
endif()

add_subdirectory(freetype)
