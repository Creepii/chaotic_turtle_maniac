#include "TextureManager.h"

#include <filesystem>

using namespace client;

client::TextureManager::TextureManager(const common::Console& console) : console(console) {
}

// void client::TextureManager::load_textures() {
//     this->load_texture("holy_salad", RESOURCE_FOLDER"/holy_salad.png");
//     this->load_texture("powerup", RESOURCE_FOLDER"/powerup.png");
//     this->load_texture("turtle", RESOURCE_FOLDER"/turtle.png");
//     this->load_texture("turtle_opening", RESOURCE_FOLDER"/turtle_hatch_opening.png");
//     this->load_texture("turtle_open", RESOURCE_FOLDER"/turtle_hatch_open.png");
//     this->load_texture("turtle_shooting", RESOURCE_FOLDER"/turtle_hatch_shooting.png");
//     this->load_texture("logo", RESOURCE_FOLDER"/logo.png");
//     this->load_texture("background", RESOURCE_FOLDER"/background.png");
// }



// public

void client::TextureManager::load_texture(const std::string& name, const std::string& filename) {
    if(this->has_texture(name)) {
        this->console.log(common::Console::ERROR, "Tried to load texture with name '" + name + "', but it already exists!");
        return;
    }

    sf::Texture loaded;
    if (!loaded.loadFromFile(filename)) {
        this->console.log(common::Console::ERROR, "Can't load texture" + filename + "!");  
        return; 
    }

    this->texture_map[name] = loaded;
}

void client::TextureManager::load_directory(const std::string& directory) {
    for(const auto& entry : std::filesystem::directory_iterator(directory)) {
        if(!entry.is_directory()) {
            if(entry.path().extension() == ".png") {
                this->load_texture(entry.path().stem().string(), entry.path().string());
                this->console.log(common::Console::INFO, "Loaded texture " + entry.path().string() + " as " + entry.path().stem().string());
            }
        }
    }
}

const sf::Texture& client::TextureManager::get_texture(const std::string& name) const {
    if (this->has_texture(name)) {
        return this->texture_map.at(name);
    } else {
        return TextureManager::get_missing_texture();
    }
}



// private

bool client::TextureManager::has_texture(const std::string& name) const {
    return this->texture_map.find(name) != this->texture_map.end();
}



// static private

const sf::Texture& client::TextureManager::get_missing_texture() {
    struct MissingTextureWrapper {
        sf::Texture missing_texture;

        MissingTextureWrapper() {
            this->missing_texture.create(16, 16);
        }
    };

    static MissingTextureWrapper missing_texture_wrapper;

    return missing_texture_wrapper.missing_texture;
}