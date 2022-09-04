#include <SFML/Graphics.hpp>
#include <random>

#include "common/Console.h"

#include "GameState.h"
#include "TextureManager.h"
#include "objects/Turtle.h"

int main() {
    std::uniform_real_distribution percentage_distrib(0.0, 1.0);
    client::TextureManager::get_instance().load_directory(RESOURCE_FOLDER);

    client::GameState game_state;

    sf::RenderWindow window(sf::VideoMode(500, 500), "Chaotic Turtle Maniac", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    window.setIcon(
        client::TextureManager::get_instance().get_texture("logo").getSize().x,
        client::TextureManager::get_instance().get_texture("logo").getSize().y,
        client::TextureManager::get_instance().get_texture("logo").copyToImage().getPixelsPtr()
    );

    // TODO: make class static game object
    sf::Sprite sprite;
    sprite.setTexture(client::TextureManager::get_instance().get_texture("holy_salad"));

    sf::Sprite background;
    background.setTexture(client::TextureManager::get_instance().get_texture("background"));
    background.setScale(
        (float)window.getSize().x / background.getTexture()->getSize().x,
        (float)window.getSize().y / background.getTexture()->getSize().y
    );

    game_state.add_game_object(std::move(std::make_unique<client::Turtle>()));

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

        std::queue<sf::Packet> received = game_state.get_socket().get_available_packets();

        while(!received.empty()) {
            float off_x, off_y;
            received.front() >> off_x >> off_y;
            received.pop();

            // get the turtle and update position
            for (std::unique_ptr<client::GameObject>& obj : game_state.get_game_objects()) {
                try {
                    client::Turtle& t = dynamic_cast<client::Turtle&>(*obj);
                    t.setPosition(t.getPosition() + sf::Vector2f{off_x, off_y});
                } catch (std::bad_cast e) {}
            }
        }

        // generate new powerup
        if (percentage_distrib(game_state.get_rand()) < 0.025) {
            std::uniform_int_distribution<unsigned int> width_distrib(0, game_state.get_window_width());
            std::uniform_int_distribution<unsigned int> height_distrib(0, game_state.get_window_height());
            sf::Vector2f loc{(float)width_distrib(game_state.get_rand()), (float)height_distrib(game_state.get_rand())};
            powerup_locations.push_back(loc);
        }

        window.clear();

        window.draw(background);
        window.draw(sprite);
        sprite.setPosition(window.getSize().x / 2 - sprite.getTexture()->getSize().x / 2, window.getSize().y / 2 - sprite.getTexture()->getSize().y / 2);

        // sf::Rect turtle_rect((sf::Vector2i) turtle.getPosition(), (sf::Vector2i) turtle.getTexture()->getSize());
        // int i = 0;
        // for (auto loc : powerup_locations) {
        //     powerup.setPosition(loc);
        //     window.draw(powerup);
        //     sf::Rect powerup_rect{(int) loc.x, (int) loc.y, (int) powerup.getTexture()->getSize().x, (int) powerup.getTexture()->getSize().y};
        //     if (powerup_rect.intersects(turtle_rect)) {
        //         powerup_locations.erase(powerup_locations.begin() + i);
        //         speed_boost += 1.0;
        //     }
        //     i++;
        // }

        // iterate all game objects
        // if tickable: tick (dynamic cast)
        for (std::unique_ptr<client::GameObject>& obj : game_state.get_game_objects()) {
            try {
                client::Tickable& t = dynamic_cast<client::Tickable&>(*obj);
                t.tick(game_state);
            } catch (std::bad_cast e) {}
            try {
                sf::Sprite& s = dynamic_cast<sf::Sprite&>(*obj);
                window.draw(s);
            } catch (std::bad_cast e) {}
        }

        window.display();
    }

    game_state.get_socket().disconnect();

    return 0;
}