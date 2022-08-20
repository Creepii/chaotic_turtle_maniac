cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(turtle_maniac)

find_package(SFML COMPONENTS window graphics system REQUIRED)

add_executable(turtle_maniac src/turtle_maniac.cpp)
target_link_libraries(turtle_maniac sfml-window sfml-graphics sfml-system)