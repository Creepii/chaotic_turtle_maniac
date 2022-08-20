cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(turtle_maniac)

find_package(SFML COMPONENTS window graphics system network REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_subdirectory(src/client)
add_subdirectory(src/server)