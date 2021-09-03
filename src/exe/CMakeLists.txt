file(GLOB_RECURSE src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake"
)

add_executable(notesharp
    ${src_files}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${src_files})

target_link_libraries(notesharp libnotesharp_loader)

if(MSVC)
    target_link_options(notesharp
        # windows平台的入口为WinMain，默认的main是带命令行的
        PRIVATE "/SUBSYSTEM:WINDOWS"
    )
endif()

# 为Windows平台增加命令行启动方式的程序，便于通过命令行调试
if(MSVC)
    add_executable(notesharpcmd ${src_files})
    target_compile_definitions(notesharpcmd PRIVATE NOTESHARP_WINDOWS_CONSOLE)
    target_link_libraries(notesharpcmd libnotesharp_loader)
endif()
