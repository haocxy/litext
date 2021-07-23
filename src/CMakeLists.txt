set(root_dir ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_AUTOMOC ON)

find_package(Boost 1.76 REQUIRED
    COMPONENTS filesystem)

include_directories(${Boost_INCLUDE_DIRS})


function(remove_useless_files_generated_for_assets)
    file(GLOB_RECURSE head_files "asset.*.h")
    foreach(head_file ${head_files})
        string(LENGTH ${head_file} len)
        math(EXPR len "${len} - 2")
        string(SUBSTRING ${head_file} 0 ${len} asset_file)
        if(NOT EXISTS ${asset_file})
            file(REMOVE ${head_file})
        endif()
    endforeach()
    file(GLOB_RECURSE cpp_files "asset.*.cpp")
    foreach(cpp_file ${cpp_files})
        string(LENGTH ${cpp_file} len)
        math(EXPR len "${len} - 4")
        string(SUBSTRING ${cpp_file} 0 ${len} asset_file)
        if(NOT EXISTS ${asset_file})
            file(REMOVE ${cpp_file})
        endif()
    endforeach()
endfunction()


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
    file(TIMESTAMP ${CMAKE_CURRENT_LIST_FILE} ts_list_file "%s")

    if(${ts_asset_file} GREATER_EQUAL ${ts_src_file})
        set(${output_var} TRUE PARENT_SCOPE)
        return()
    endif()

    # 如果当前CMakeLists.txt文件是新的，可能逻辑有变化，也需要为资源文件重新生成C++文件
    if(${ts_list_file} GREATER_EQUAL ${ts_src_file})
        set(${output_var} TRUE PARENT_SCOPE)
        return()
    endif()

    set(${output_var} FALSE PARENT_SCOPE)
endfunction()


function(make_cpp_ns_name_for_asset output_var ${asset_file})
    set(${output_var} "Asset" PARENT_SCOPE)
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
    message(STATUS "Generate head file [${head_file}]")

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
    message(STATUS "Generate cpp file [${cpp_file}]")

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
    file(APPEND ${cpp_file} "    const unsigned char ${var_name}[] = {")
    file(SIZE ${asset_file} file_size)
    if(${file_size} GREATER 0)
        file(APPEND ${cpp_file} "\n")
        file(READ ${asset_file} hex_data_str HEX)
        set(str_len 0)
        string(LENGTH ${hex_data_str} str_len)
        math(EXPR last_index "${str_len} - 1")
        set(arr_data "")
        set(is_first_item TRUE)
        foreach(index RANGE 0 ${last_index} 2)
            string(SUBSTRING ${hex_data_str} ${index} 2 hex_item)
            math(EXPR dec_item "0x${hex_item}")
            string(APPEND arr_data ${dec_item})
            string(APPEND arr_data ",")
            set(is_first_item FALSE)
            string(LENGTH ${arr_data} len)
            if(${len} GREATER_EQUAL 100)
                string(APPEND arr_data "\n")
                file(APPEND ${cpp_file} ${arr_data})
                set(arr_data "")
            endif()
        endforeach()

        file(APPEND ${cpp_file} "${arr_data}0\n")
        file(APPEND ${cpp_file} "    };\n")
    else()
        file(APPEND ${cpp_file} "0};\n")
    endif()
    
    
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


remove_useless_files_generated_for_assets()
generate_src_file_for_all_assets()

include_directories(${root_dir})
include_directories(${CMAKE_SOURCE_DIR})

if(MSVC)
    include_directories($ENV{LIBCAIRO_INCLUDE})
endif()

file(GLOB_RECURSE src_files
    "${root_dir}/*.h"
    "${root_dir}/*.cpp"
    "${root_dir}/*.txt"
    "${root_dir}/*.sql"
)

add_executable(notesharp
    ${src_files}
)

if(MSVC)
    target_link_options(notesharp
        # windows平台的入口为WinMain，默认的main是带命令行的
        PRIVATE "/SUBSYSTEM:WINDOWS"
    )
endif()

source_group(TREE "${root_dir}/.." FILES ${src_files})

set(deps
    Qt5::Widgets
    libuchardet_static
    libsqlite3_static
    ${Boost_LIBRARIES}
)

if(MSVC)
    list(APPEND deps $ENV{LIBCAIRO_LIB})
else()
    list(APPEND deps dl)
    list(APPEND deps cairo)
endif()

target_link_libraries(notesharp
    ${deps}
)
