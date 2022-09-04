/*
Turtle: inherits Animation
- set animation parameters
- movement (need tickable)
- active powerups
- collision on move
*/

#pragma once

#include "../Animation.h"
#include "Tickable.h"
#include "../GameState.h"
#include "GameObject.h"

#define TURTLE_ANIMATION_TEXTURES {"turtle", "turtle_hatch_opening", "turtle_hatch_open", "turtle_hatch_shooting", "turtle_hatch_open", "turtle_hatch_opening"}
#define TURTLE_ANIMATION_TIMES {1000, 300, 300, 100, 300, 300}

namespace client {
    class Turtle : public client::Animation, public client::Tickable, public client::GameObject {
        private:
            double speed_boost;

            void send_position_change(client::GameState& game_state, float off_x, float off_y);
            sf::Vector2f get_turtle_move(client::GameState& game_state);
        public:
            Turtle();
            Turtle(Turtle&&) = default;
            void tick(client::GameState& game_state) override;
    };
}