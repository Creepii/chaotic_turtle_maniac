#pragma once

#include <random>
#include <vector>
#include <memory>

#include "network/ClientSocket.h"
#include "common/Console.h"
#include "objects/GameObject.h"

namespace client {
    class GameState {
        private:
            int window_width, window_height;
            client::network::ClientSocket socket;
            std::vector<std::unique_ptr<client::GameObject>> game_objects;

            std::default_random_engine rand_engine;
        public:
            GameState();
            client::network::ClientSocket& get_socket();
            int get_window_width() const;
            int get_window_height() const;
            std::default_random_engine& get_rand();
            void add_game_object(std::unique_ptr<client::GameObject> obj);
            std::vector<std::unique_ptr<client::GameObject>>& get_game_objects();
    };
}