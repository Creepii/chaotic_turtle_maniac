#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>

#include "common/Console.h"

namespace client {
    class TextureManager {
        private:
            static TextureManager singleton;

            std::map<std::string, sf::Texture> texture_map;

            bool has_texture(const std::string& name) const;

            static const sf::Texture& get_missing_texture();

            TextureManager();
        public:
            void load_texture(const std::string& name, const std::string& filename);
            void load_directory(const std::string& directory);

            static TextureManager& get_instance();
            const sf::Texture& get_texture(const std::string& name) const;
    };
}