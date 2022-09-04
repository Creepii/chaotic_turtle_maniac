#pragma once

#include <random>

#include "network/ClientSocket.h"
#include "common/Console.h"

namespace client {
    class GameState {
        private:
            int window_width, window_height;
            client::network::ClientSocket socket;

            std::default_random_engine rand_engine;
        public:
            GameState();
            client::network::ClientSocket& get_socket();
            int get_window_width() const;
            int get_window_height() const;
            std::default_random_engine& get_rand();
    };
}