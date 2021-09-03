if(NOT MSVC)
    add_compile_options(-fPIC)
endif()

add_library(libsqlite3_static STATIC
    sqlite3.c)
