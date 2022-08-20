find_package(SFML COMPONENTS window graphics system REQUIRED)

add_executable(turtle_maniac_client turtle_maniac_client.cpp)
target_link_libraries(turtle_maniac_client sfml-window sfml-graphics sfml-system)