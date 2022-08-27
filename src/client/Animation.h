#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <chrono>

#include "TextureManager.h"

class Animation : public sf::Sprite {
    private:
        std::vector<std::string> textures;
        std::vector<int> cooldowns;
        int current;
        std::chrono::time_point<std::chrono::system_clock> start_time;
        bool playing;
        bool repeating;
    public:
        Animation(std::vector<std::string> textures, std::vector<int> cooldowns);
        void play();
        void tick();
        void set_repeating(bool value);
};