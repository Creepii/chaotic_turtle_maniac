#include "TextureManager.h"

TextureManager::TextureManager() {
    if (!this->missing.create(1, 1)) {
        std::cout << "Severe error" << std::endl;
    }
}

void TextureManager::load_textures() {
    this->load_texture("holy_salad", "../res/holy_salad.png");
    this->load_texture("powerup", "../res/powerup.png");
    this->load_texture("turtle", "../res/turtle.png");
    this->load_texture("turtle_opening", "../res/turtle_hatch_opening.png");
    this->load_texture("turtle_open", "../res/turtle_hatch_open.png");
    this->load_texture("turtle_shooting", "../res/turtle_hatch_shooting.png");
    this->load_texture("logo", "../res/logo.png");
}

void TextureManager::load_texture(std::string name, std::string filename) {
    sf::Texture texture;
    if (!texture.loadFromFile(filename)) {
        std::cout << "Cannot load texture " << filename << std::endl;
    }
    this->texture_map[name] = texture;
}

const sf::Texture& TextureManager::get_texture(std::string name) {
    if (this->texture_map.find(name) != this->texture_map.end()) {
        return this->texture_map[name];
    } else {
        return this->missing;
    }
}