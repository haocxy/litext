execute_process(
    COMMAND git clone https://gitee.com/haocxy/libiconv.git -b v1.16 --depth 1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

