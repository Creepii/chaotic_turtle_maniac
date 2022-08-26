find_package(SFML COMPONENTS window graphics system REQUIRED)

add_executable(tm_client tm_client.cpp TextureManager.cpp)
target_link_libraries(tm_client sfml-window sfml-graphics sfml-system)