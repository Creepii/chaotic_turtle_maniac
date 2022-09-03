#pragma once

#include "../GameState.h"

namespace client {
    class Tickable {
        public:
            Tickable() = default;
            virtual ~Tickable() = default;
            virtual void tick(client::GameState& game_state) = 0;
    };
}