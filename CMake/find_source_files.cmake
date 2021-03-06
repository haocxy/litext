function(isSrcForCurPlat out_var file_path)
    if(${file_path} MATCHES "^.+\\.win32\\.(h|cpp)")
        if(MSVC)
            set(${out_var} TRUE PARENT_SCOPE)
            return()
        else()
            set(${out_var} FALSE PARENT_SCOPE)
            return()
        endif()
    elseif(${file_path} MATCHES "^.+\\.unix\\.(h|cpp)")
        if(MSVC)
            set(${out_var} FALSE PARENT_SCOPE)
            return()
        else()
            set(${out_var} TRUE PARENT_SCOPE)
            return()
        endif()
    elseif(${file_path} MATCHES "^.+\\.(h|cpp)")
        set(${out_var} TRUE PARENT_SCOPE)
        return()
    else()
        set(${out_var} FALSE PARENT_SCOPE)
        return()
    endif()
endfunction()

function(findSourceFiles output_files)
    file(GLOB_RECURSE cpp_files
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    )
    set(result "")
    foreach(cpp_file ${cpp_files})
        isSrcForCurPlat(should_add ${cpp_file})
        if(${should_add})
            list(APPEND result ${cpp_file})
        endif()
    endforeach()
    file(GLOB_RECURSE other_files
        "${CMAKE_CURRENT_SOURCE_DIR}/*.txt"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.sql"
    )
    list(APPEND result ${other_files})
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${result})
    set(${output_files} ${result} PARENT_SCOPE)
endfunction()
