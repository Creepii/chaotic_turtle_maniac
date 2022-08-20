find_package(SFML COMPONENTS network REQUIRED)

set(SERVER_SOURCE_FILES
    TurtleManiacServer.cpp
)

add_executable(tm_server tm_server.cpp ${SERVER_SOURCE_FILES})
target_link_libraries(tm_server sfml-network)