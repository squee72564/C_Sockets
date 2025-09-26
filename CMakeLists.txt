cmake_minimum_required(VERSION 3.10)

project(sockets_c CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(showip_test tests/showip.cpp)

add_executable(echo_server_test tests/echo_server.cpp)
add_executable(echo_client_test tests/echo_client.cpp)
