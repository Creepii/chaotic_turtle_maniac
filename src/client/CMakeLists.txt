find_package(SFML COMPONENTS window graphics system REQUIRED)

set(CLIENT_SOURCE_FILES
    TextureManager.cpp
    
    network/ClientSocket.cpp
)

add_executable(tm_client tm_client.cpp ${CLIENT_SOURCE_FILES})
target_link_libraries(tm_client sfml-window sfml-graphics sfml-system sfml-network common readline)
target_include_directories(tm_client PRIVATE ${SRC_INCLUDE_FOLDER} /usr/include/readline)