#include "TextureManager.h"

TextureManager::TextureManager() {
    if (!this->missing.create(1, 1)) {
        std::cout << "Severe error" << std::endl;
    }
}

void TextureManager::load_textures() {
    this->load_texture("holy_salad", RESOURCE_FOLDER"/holy_salad.png");
    this->load_texture("powerup", RESOURCE_FOLDER"/powerup.png");
    this->load_texture("turtle", RESOURCE_FOLDER"/turtle.png");
    this->load_texture("turtle_opening", RESOURCE_FOLDER"/turtle_hatch_opening.png");
    this->load_texture("turtle_open", RESOURCE_FOLDER"/turtle_hatch_open.png");
    this->load_texture("turtle_shooting", RESOURCE_FOLDER"/turtle_hatch_shooting.png");
    this->load_texture("logo", RESOURCE_FOLDER"/logo.png");
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