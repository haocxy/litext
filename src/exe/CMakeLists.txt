file(GLOB_RECURSE src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${src_files})

if(MSVC)
    list(APPEND src_files ${CMAKE_SOURCE_DIR}/windows.rc)
endif()

add_executable(litext
    ${src_files}
)

target_link_libraries(litext litext_loader)

if(MSVC)
    target_link_options(litext
        # windows平台的入口为WinMain，默认的main是带命令行的
        PRIVATE "/SUBSYSTEM:WINDOWS"
    )
    add_custom_command(TARGET litext POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:litext> $<TARGET_FILE_DIR:litext>
        COMMAND_EXPAND_LISTS
    )
endif()

# 为Windows平台增加命令行启动方式的程序，便于通过命令行调试
if(MSVC)
    add_executable(litextcmd ${src_files})
    target_compile_definitions(litextcmd PRIVATE NOTESHARP_WINDOWS_CONSOLE)
    target_link_libraries(litextcmd litext_loader)
    add_custom_command(TARGET litextcmd POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:litext> $<TARGET_FILE_DIR:litextcmd>
        COMMAND_EXPAND_LISTS
    )
endif()


