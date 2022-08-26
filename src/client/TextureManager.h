#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <iostream>

class TextureManager {
    private:
        std::map<std::string, sf::Texture> texture_map;
        sf::Texture missing;

        void load_texture(std::string name, std::string filename);
    public:
        TextureManager();
        void load_textures();
        const sf::Texture& get_texture(std::string name);
};