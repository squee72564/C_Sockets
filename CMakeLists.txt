cmake_minimum_required(VERSION 3.10)

project(sockets_c CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wpedantic")

add_library(network_helper INTERFACE)
target_include_directories(network_helper INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(showip_test tests/showip.cpp)
target_link_libraries(showip_test PRIVATE network_helper)

add_executable(echo_server_test tests/echo_server.cpp)
target_link_libraries(echo_server_test PRIVATE network_helper)

add_executable(echo_client_test tests/echo_client.cpp)
target_link_libraries(echo_client_test PRIVATE network_helper)


include_directories(showip_test PRIVATE )
