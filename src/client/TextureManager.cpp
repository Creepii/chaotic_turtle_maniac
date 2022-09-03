#include "TextureManager.h"

#include <filesystem>

using namespace client;

client::TextureManager client::TextureManager::singleton = client::TextureManager{};

// private

client::TextureManager::TextureManager() {
}

// static
client::TextureManager& client::TextureManager::get_instance() {
    return client::TextureManager::singleton;
}


// public

void client::TextureManager::load_texture(const std::string& name, const std::string& filename) {
    if(this->has_texture(name)) {
        common::Console::get_instance().log(common::Console::ERROR, "Tried to load texture with name '" + name + "', but it already exists!");
        return;
    }

    sf::Texture loaded;
    if (!loaded.loadFromFile(filename)) {
        common::Console::get_instance().log(common::Console::ERROR, "Can't load texture" + filename + "!");  
        return; 
    }

    this->texture_map[name] = loaded;
}

void client::TextureManager::load_directory(const std::string& directory) {
    for(const auto& entry : std::filesystem::directory_iterator(directory)) {
        if(!entry.is_directory()) {
            if(entry.path().extension() == ".png") {
                this->load_texture(entry.path().stem().string(), entry.path().string());
                common::Console::get_instance().log(common::Console::INFO, "Loaded texture " + entry.path().string() + " as " + entry.path().stem().string());
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