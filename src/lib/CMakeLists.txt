set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 使用 Qt UIC 生成C++文件
# 不使用 cmake 自带的 CMAKE_AUTOUIC 功能,
# 因为 cmake 自带的 CMAKE_AUTOUIC 直到构建才生成C++文件,
# 使用IDE开发时很麻烦
find_program(quic_exe uic)
if(${quic_exe} STREQUAL "quic_exe-NOTFOUND")
    message(FATAL_ERROR "Cannot find Qt uic executable")
endif()
message(STATUS "Qt uic executable found: ${quic_exe}")

find_package(Python3 COMPONENTS Interpreter REQUIRED)
message(STATUS "Python3 interpreter found: ${Python3_EXECUTABLE}")

message(STATUS "Updating Qt UIC")
execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tools/update_qt_uic.py" "${quic_exe}" "${CMAKE_CURRENT_SOURCE_DIR}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
message(STATUS "Qt UIC updated")


file(GLOB_RECURSE src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.sql"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${src_files})

add_library(litext_body SHARED ${src_files} ${CMAKE_SOURCE_DIR}/litext_resources.qrc)

target_link_libraries(litext_body litext_global)
target_link_libraries(litext_body litext_charset)


# dep Qt5
find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)

set_target_properties(Qt5::Core PROPERTIES MAP_IMPORTED_CONFIG_DEBUG "RELEASE")
set_target_properties(Qt5::Gui PROPERTIES MAP_IMPORTED_CONFIG_DEBUG "RELEASE")
set_target_properties(Qt5::Widgets PROPERTIES MAP_IMPORTED_CONFIG_DEBUG "RELEASE")
set_target_properties(Qt5::Network PROPERTIES MAP_IMPORTED_CONFIG_DEBUG "RELEASE")
target_link_libraries(litext_core Qt5::Widgets Qt5::Network)

