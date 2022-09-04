#include "GameState.h"

client::GameState::GameState()
 : window_width{500}, window_height{500}, socket{"127.0.0.1", 50141} {
    socket.connect();
    std::random_device trd;
    this->rand_engine = std::default_random_engine(trd());
}

client::network::ClientSocket& client::GameState::get_socket() {
    return this->socket;
}

int client::GameState::get_window_width() const {
    return this->window_width;
}

int client::GameState::get_window_height() const {
    return this->window_height;
}

std::default_random_engine& client::GameState::get_rand() {
    return this->rand_engine;
}

void client::GameState::add_game_object(std::unique_ptr<client::GameObject> obj) {
    this->game_objects.push_back(std::move(obj));
}

std::vector<std::unique_ptr<client::GameObject>>& client::GameState::get_game_objects() {
    return this->game_objects;
}