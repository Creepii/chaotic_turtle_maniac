find_package(SFML COMPONENTS network REQUIRED)

set(SERVER_SOURCE_FILES
    TurtleManiacServer.cpp
)

add_executable(turtle_maniac_server turtle_maniac_server.cpp ${SERVER_SOURCE_FILES})
target_link_libraries(turtle_maniac_server sfml-network)