cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(turtle_maniac)
add_compile_definitions(RESOURCE_FOLDER="${CMAKE_SOURCE_DIR}/res")

find_package(SFML COMPONENTS window graphics system network REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(SRC_INCLUDE_FOLDER ${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src/common)
add_subdirectory(src/client)
add_subdirectory(src/server)