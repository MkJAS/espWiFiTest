cmake_minimum_required(VERSION 3.5.0)
project(server VERSION 0.1.0 LANGUAGES C CXX)

enable_testing()

add_executable(server server.cpp)
add_executable(client client.cpp)

set_target_properties(server client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


