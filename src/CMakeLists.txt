set(root_dir ${CMAKE_CURRENT_SOURCE_DIR})


function(check_should_generate_src_file_for_asset output_var asset_file src_file)
    if(NOT EXISTS ${asset_file})
        set(${output_var} FALSE PARENT_SCOPE)
        return()
    endif()
    if(NOT EXISTS ${src_file})
        set(${output_var} TRUE PARENT_SCOPE)
        return()
    endif()
    file(TIMESTAMP ${asset_file} ts_asset_file "%s")
    file(TIMESTAMP ${src_file} ts_src_file "%s")
    if(${ts_asset_file} GREATER_EQUAL ${ts_src_file})
        set(${output_var} TRUE PARENT_SCOPE)
    else()
        set(${output_var} FALSE PARENT_SCOPE)
    endif()
endfunction()


function(make_cpp_ns_name_for_asset output_var ${asset_file})
    string(LENGTH ${CMAKE_CURRENT_SOURCE_DIR} dir_len)
    string(SUBSTRING ${asset_file} ${dir_len} -1 relative_path)
    while(${relative_path} MATCHES "^(\\\\|/).*$")
        string(SUBSTRING ${relative_path} 1 -1 relative_path)
    endwhile()

    get_filename_component(relative_dir ${relative_path} DIRECTORY)
    # TODO 这里逻辑需要强化下，现在先假设CMake提供的路径在所有平台都用 / 分割，且路径中的目录名均为有效的C++标识符
    message(WARNING "The logic should be optimized, please look at comment in cmake file")
    string(REPLACE "/" "::" ns_name ${relative_dir})
    string(APPEND ns_name "::Asset")
    set(${output_var} ${ns_name} PARENT_SCOPE)
endfunction()


function(make_cpp_var_name_for_asset output_var ${asset_file})
    get_filename_component(file_name ${asset_file} NAME)

    set(asset_file_name_regex "^asset\\..*$")
    if(NOT(${file_name} MATCHES ${asset_file_name_regex}))
        message(FATAL_ERROR "bad asset file name of [${asset_file}] by regex [${asset_file_name_regex}]")
        set(${var_name} "" PARENT_SCOPE)
        return()
    endif()

    string(LENGTH "asset." suffix_len)
    string(SUBSTRING ${file_name} ${suffix_len} -1 name_no_suffix)
    string(REPLACE "." "__" name_no_suffix ${name_no_suffix})
    string(MAKE_C_IDENTIFIER ${name_no_suffix} var_name)
    set(${output_var} ${var_name} PARENT_SCOPE)
endfunction()


function(generate_head_file_for_asset ${head_file} ${asset_file})
    message(STATUS "Generate head file [${head_file}] for asset file [${asset_file}]")

    file(REMOVE ${head_file})
    make_cpp_ns_name_for_asset(ns_name ${asset_file})
    make_cpp_var_name_for_asset(var_name ${asset_file})

    get_filename_component(asset_file_name ${asset_file} NAME)

    file(APPEND ${head_file} "// This file is generated automatically\n")
    file(APPEND ${head_file} "// for asset file: \'${asset_file_name}\'\n")
    file(APPEND ${head_file} "#include <cstddef>\n")
    file(APPEND ${head_file} "\n")
    file(APPEND ${head_file} "\n")
    file(APPEND ${head_file} "namespace ${ns_name}::Data {\n")
    file(APPEND ${head_file} "    extern const unsigned char ${var_name}[];\n")
    file(APPEND ${head_file} "}\n")
    file(APPEND ${head_file} "\n")
    file(APPEND ${head_file} "namespace ${ns_name}::Len {\n")
    file(APPEND ${head_file} "    extern const size_t ${var_name};\n")
    file(APPEND ${head_file} "}\n")
    file(APPEND ${head_file} "\n")
endfunction()


function(generate_cpp_file_for_asset ${head_file} ${cpp_file} ${asset_file})
    message(STATUS "Generate cpp file [${cpp_file}] for asset file [${asset_file}]")

    file(REMOVE ${cpp_file})
    make_cpp_ns_name_for_asset(ns_name ${asset_file})
    make_cpp_var_name_for_asset(var_name ${asset_file})

    get_filename_component(head_file_name ${head_file} NAME)
    get_filename_component(asset_file_name ${asset_file} NAME)

    file(APPEND ${cpp_file} "// This file is generated automatically\n")
    file(APPEND ${cpp_file} "// for asset file: \'${asset_file_name}\'\n")
    file(APPEND ${cpp_file} "#include \"${head_file_name}\"\n")
    file(APPEND ${cpp_file} "\n")
    file(APPEND ${cpp_file} "\n")
    file(APPEND ${cpp_file} "namespace ${ns_name}::Data {\n")

    file(READ ${asset_file} hex_asset_data HEX)
    string(REGEX REPLACE ".." "0x\\0, " cpp_hex_asset_data ${hex_asset_data})

    file(APPEND ${cpp_file} "    const unsigned char ${var_name}[] = {\n")
    file(APPEND ${cpp_file} "        ${cpp_hex_asset_data}0x00\n")
    file(APPEND ${cpp_file} "    };\n")
    file(APPEND ${cpp_file} "}\n")
    file(APPEND ${cpp_file} "\n")
    file(APPEND ${cpp_file} "namespace ${ns_name}::Len {\n")
    file(SIZE ${asset_file} file_size)
    file(APPEND ${cpp_file} "    const size_t ${var_name} = ${file_size};\n")
    file(APPEND ${cpp_file} "}\n")
    file(APPEND ${cpp_file} "\n")
endfunction()


function(generate_src_file_for_one_asset asset_file)
    string(CONCAT head_file ${asset_file} ".h")
    string(CONCAT cpp_file ${asset_file} ".cpp")

    check_should_generate_src_file_for_asset(should_generate_head ${asset_file} ${head_file})
    if(${should_generate_head})
        generate_head_file_for_asset(${head_file} ${asset_file})
    endif()

    check_should_generate_src_file_for_asset(should_generate_cpp ${asset_file} ${cpp_file})
    if(${should_generate_cpp})
        generate_cpp_file_for_asset(${head_file} ${cpp_file} ${asset_file})
    endif()
endfunction()


function(generate_src_file_for_all_assets)
    message(STATUS "Generate source files for asset files")
    file(GLOB_RECURSE asset_files "asset.*")
    foreach(asset_file ${asset_files})
        if(NOT(${asset_file} MATCHES "^\.*\\.(h|cpp)$"))
            generate_src_file_for_one_asset(${asset_file})
        endif()
    endforeach()
endfunction()


generate_src_file_for_all_assets()

include_directories(${root_dir})
include_directories(${CMAKE_SOURCE_DIR})

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
    libuchardet_static)
