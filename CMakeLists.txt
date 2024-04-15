cmake_minimum_required(VERSION 3.10)
project(cppserver)

# 设置要编译的源文件列表
file(GLOB SOURCES "src/*.cpp")

# 添加可执行文件并将源文件与之关联
add_executable(client client.cpp ${SOURCES})
add_executable(server server.cpp ${SOURCES})
add_executable(test test.cpp ${SOURCES})

# 包含源文件的头文件目录
include_directories(src/include)

# 链接 pthread 库
target_link_libraries(client pthread)
target_link_libraries(server pthread)
target_link_libraries(test pthread)

# 添加格式检查和代码风格规范检查的自定义构建目标
add_custom_target(format
    COMMAND clang-format -i -style=file ${SOURCES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running clang-format"
)

add_custom_target(clang-tidy
    COMMAND ${CLANG_TIDY_BIN} -p ${CMAKE_BINARY_DIR} ${SOURCES} -fix
    COMMENT "Running clang-tidy to check code style"
)

add_custom_target(run-cpplint
    COMMAND ${CPPLINT_BIN} --verbose=2 --quiet
    --filter=-legal/copyright,-build/include_subdir,-readability/utf8,-whitespace/ending_newline ${SOURCES}
    COMMENT "Running cpplint to check code style"
)


# 添加格式检查和代码风格规范检查到构建过程中
add_dependencies(format client server test)
add_dependencies(clang-tidy client server test)
add_dependencies(run-cpplint client server test)