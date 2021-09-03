if(MSVC)
    add_compile_options(/w)
else()
    add_compile_options(-w)
endif()

add_subdirectory(freetype_wrapper)
add_subdirectory(sqlite3)
add_subdirectory(uchardet_wrapper)
