cmake_minimum_required(VERSION 3.10)
project(cppserver)

file(GLOB SOURCES "src/*.cpp")

add_executable(client client.cpp src/util.cpp)
add_executable(server server.cpp ${SOURCES})

include_directories(src)

target_link_libraries(client)
target_link_libraries(server)