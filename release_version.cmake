
function(get_git_all_tag_refnames outvar)
    execute_process(
        COMMAND git tag -l "--format=%(refname)"
        OUTPUT_VARIABLE cmdout
    )
    string(REGEX REPLACE "[ \r\n]+" ";" tagrefnames ${cmdout})
    set(${outvar} ${tagrefnames} PARENT_SCOPE)
endfunction()

function(get_git_obj_id_by_refname refname outvar)
    execute_process(
        COMMAND git show "${refname}" "-s" "--format=%H"
        OUTPUT_VARIABLE cmdout
    )
    string(STRIP ${cmdout} obj_id)
    set(${outvar} ${obj_id} PARENT_SCOPE)
endfunction()

function(get_release_version outvar)
    get_git_all_tag_refnames(tagrefnames)
    get_git_obj_id_by_refname(HEAD head_obj_id)
    foreach(tagrefname ${tagrefnames})
        string(STRIP ${tagrefname} tagrefname)
        if(NOT(${tagrefname} MATCHES "refs/tags/v[0-9]+\\.[0-9]+\\.[0-9]+"))
            continue()
        endif()
        get_git_obj_id_by_refname(${tagrefname} tag_obj_id)
        if(${head_obj_id} STREQUAL ${tag_obj_id})
            string(REGEX REPLACE "^refs/tags/v" "" short_ref_name ${tagrefname})
            set(${outvar} ${short_ref_name} PARENT_SCOPE)
            return()
        endif()
    endforeach()
    set(${outvar} ${head_obj_id} PARENT_SCOPE)
endfunction()

get_release_version(release_version)
