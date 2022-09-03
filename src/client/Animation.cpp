#include "Animation.h"

using namespace client;

client::Animation::Animation(const std::vector<std::string>& textures, const std::vector<int>& cooldowns) :
    current{0},
    textures{textures},
    cooldowns{cooldowns},
    playing{false},
    repeating{false},
    start_time{std::chrono::system_clock::now()} {
        this->setTexture(client::TextureManager::get_instance().get_texture(this->textures[0]));
    }

void client::Animation::play() {
    this->playing = true;
    this->start_time = std::chrono::system_clock::now();
}

void client::Animation::tick() {
    using namespace std::literals;

    if (this->playing) {
        int time_passed = (std::chrono::system_clock::now() - start_time) / 1ms;
        if (time_passed > this->cooldowns[current]) {
            this->current++;
            if (this->current >= this->textures.size()) {
                this->playing = false;
                this->current = 0;
            } else {
                this->start_time = std::chrono::system_clock::now();
            }
            this->setTexture(client::TextureManager::get_instance().get_texture(this->textures[this->current]));
        }
    } else if (this->repeating) {
        this->play();
    }
}

void client::Animation::set_repeating(bool value) {
    this->repeating = value;
}