file(GLOB_RECURSE src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake"
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${src_files})

# 这个模块为程序全局服务,例如工作目录等服务端和客户端都用到的逻辑
add_library(litext_global STATIC ${src_files})

# dep Boost
find_package(Boost ${MY_BOOST_VER} REQUIRED COMPONENTS serialization HINTS ${MY_BOOST_ROOT})
target_link_libraries(litext_global ${Boost_LIBRARIES})

target_link_libraries(litext_global litext_core)
