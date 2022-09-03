#pragma once

#include "network/ClientSocket.h"
#include "common/Console.h"

namespace client {
    class GameState {
        private:
            int window_width, window_height;
            client::network::ClientSocket socket;
        public:
            GameState();
            client::network::ClientSocket& get_socket();
            const int& get_window_width();
            const int& get_window_height();
    };
}