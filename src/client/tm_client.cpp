#include <SFML/Graphics.hpp>
#include <random>

#include "GameState.h"
#include "TextureManager.h"
#include "objects/Turtle.h"

// inclusive min and max
int get_random_int(int min, int max) {
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(min, max);
    return uniform_dist(e1);
}

int main() {
    client::TextureManager::get_instance().load_directory(RESOURCE_FOLDER);

    client::GameState game_state;

    sf::RenderWindow window(sf::VideoMode(500, 500), "Chaotic Turtle Maniac", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    window.setIcon(
        client::TextureManager::get_instance().get_texture("logo").getSize().x,
        client::TextureManager::get_instance().get_texture("logo").getSize().y,
        client::TextureManager::get_instance().get_texture("logo").copyToImage().getPixelsPtr()
    );

    sf::Sprite sprite;
    sprite.setTexture(client::TextureManager::get_instance().get_texture("holy_salad"));

    sf::Sprite background;
    background.setTexture(client::TextureManager::get_instance().get_texture("background"));
    background.setScale(
        (float)window.getSize().x / background.getTexture()->getSize().x,
        (float)window.getSize().y / background.getTexture()->getSize().y
    );

    client::Turtle turtle;

    std::vector<sf::Vector2f> powerup_locations;
    sf::Sprite powerup;
    powerup.setTexture(client::TextureManager::get_instance().get_texture("powerup"));
    double speed_boost = 1.0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
        }

        // generate new dingus
        if (get_random_int(0, 500) == 0) {
            sf::Vector2f loc{(float)get_random_int(0, window.getSize().x), (float)get_random_int(0, window.getSize().y)};
            powerup_locations.push_back(loc);
        }

        window.clear();

        window.draw(background);
        window.draw(sprite);
        sprite.setPosition(window.getSize().x / 2 - sprite.getTexture()->getSize().x / 2, window.getSize().y / 2 - sprite.getTexture()->getSize().y / 2);

        sf::Rect turtle_rect((sf::Vector2i) turtle.getPosition(), (sf::Vector2i) turtle.getTexture()->getSize());
        int i = 0;
        for (auto loc : powerup_locations) {
            powerup.setPosition(loc);
            window.draw(powerup);
            sf::Rect powerup_rect{(int) loc.x, (int) loc.y, (int) powerup.getTexture()->getSize().x, (int) powerup.getTexture()->getSize().y};
            if (powerup_rect.intersects(turtle_rect)) {
                powerup_locations.erase(powerup_locations.begin() + i);
                speed_boost += 1.0;
            }
            i++;
        }

        window.draw(turtle);
        turtle.tick(game_state);

        window.display();
    }

    game_state.get_socket().disconnect();

    return 0;
}