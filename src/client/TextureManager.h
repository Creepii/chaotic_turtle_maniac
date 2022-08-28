#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>

#include "common/Console.h"

namespace client {
    class TextureManager {
        private:
            const common::Console& console;
            std::map<std::string, sf::Texture> texture_map;

            bool has_texture(const std::string& name) const;

            static const sf::Texture& get_missing_texture();
        public:
            TextureManager(const common::Console& console);
            void load_texture(const std::string& name, const std::string& filename);
            void load_directory(const std::string& directory);
            const sf::Texture& get_texture(const std::string& name) const;
    };
}