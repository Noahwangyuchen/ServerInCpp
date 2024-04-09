cmake_minimum_required(VERSION 3.10)
project(cppserver)

add_executable(client src/client.cpp src/util.cpp)
add_executable(server src/server.cpp src/util.cpp src/Epoll.cpp src/InetAddress.cpp src/Socket.cpp)

include_directories(src)

target_link_libraries(client)
target_link_libraries(server)