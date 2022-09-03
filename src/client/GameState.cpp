#include "GameState.h"

client::GameState::GameState() :
    window_width{500},
    window_height{500},
    socket{"127.0.0.1", 50141} {
        socket.connect();
}

client::network::ClientSocket& client::GameState::get_socket() {
    return this->socket;
}

const int& client::GameState::get_window_width() {
    return this->window_width;
}

const int& client::GameState::get_window_height() {
    return this->window_height;
}