#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <chrono>

#include "TextureManager.h"

namespace client {
    class Animation : public sf::Sprite {
        private:
            const std::vector<std::string> textures;
            const std::vector<int> cooldowns;

            const client::TextureManager& texture_manager;

            int current;
            std::chrono::time_point<std::chrono::system_clock> start_time;
            bool playing;
            bool repeating;
        public:
            Animation(const std::vector<std::string>& textures, const std::vector<int>& cooldowns, const client::TextureManager& texture_manager);
            void play();
            void tick();
            void set_repeating(bool value);
    };
}