set(BUILD_SHARED_LIBS OFF)

if(NOT MSVC)
    add_compile_options(-fPIC)
endif()

add_subdirectory(uchardet-0.0.7)
