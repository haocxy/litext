set(BUILD_STATIC TRUE)

if(NOT MSVC)
    add_compile_options(-fPIC)
endif()

add_subdirectory(uchardet-0.0.7)
