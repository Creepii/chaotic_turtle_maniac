#include <SFML/Graphics.hpp>

#include "network/ClientSocket.h"

#include "TextureManager.h"
#include "Animation.h"

// fraction: 10 = 10th of screen, 5 = 5th of screen
double calc_scale(const sf::RenderWindow& window, const sf::Texture* texture, double fraction) {
    int win_size = window.getSize().x > window.getSize().y ? window.getSize().x : window.getSize().y;
    double sprite_size = win_size / fraction;
    return sprite_size / texture->getSize().x;
}

int main() {
    common::Console console{};
    client::network::ClientSocket socket("127.0.0.1", 50141, console);

    socket.connect();

    sf::RenderWindow window(sf::VideoMode(500, 500), "Crazy Turtle Maniac");
    window.setFramerateLimit(60);
    window.setIcon(texture_manager.get_texture("logo").getSize().x, texture_manager.get_texture("logo").getSize().y, texture_manager.get_texture("logo").copyToImage().getPixelsPtr());
    sf::Sprite sprite;
    sprite.setTexture(texture_manager.get_texture("holy_salad"));

    std::vector<std::string> tex = {"turtle", "turtle_opening", "turtle_open", "turtle_shooting", "turtle_open", "turtle_opening"};
    std::vector<int> time = {1000, 300, 300, 100, 300, 300};
    Animation turtle{tex, time};
    turtle.set_repeating(true);

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

        double turtle_scale = calc_scale(window, turtle.getTexture(), 16);

        float move_dist = window.getSize().x > window.getSize().y ? window.getSize().x / 100.0 : window.getSize().y / 100.0;
        sf::Vector2f new_pos = turtle.getPosition();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            new_pos -= sf::Vector2f(0.0 , move_dist);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            new_pos += sf::Vector2f(0.0 , move_dist);
        }
        if (new_pos.y >= 0 && new_pos.y <= (window.getSize().y - turtle.getTexture()->getSize().y * turtle_scale)) {
            turtle.setPosition(new_pos);
        }

        new_pos = turtle.getPosition();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            new_pos -= sf::Vector2f(move_dist , 0.0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            new_pos += sf::Vector2f(move_dist , 0.0);
        }
        if (new_pos.x >= 0 && new_pos.x <= (window.getSize().x - turtle.getTexture()->getSize().x * turtle_scale)) {
            turtle.setPosition(new_pos);
        }

        window.clear();

        window.draw(sprite);
        double scale = calc_scale(window, sprite.getTexture(), 16);
        sprite.setScale(scale, scale);
        sprite.setPosition(window.getSize().x / 2 - sprite.getTexture()->getSize().x * scale / 2, window.getSize().y / 2 - sprite.getTexture()->getSize().y * scale / 2);

        turtle.setScale(turtle_scale, turtle_scale);
        window.draw(turtle);
        turtle.tick();

        window.display();
    }

    socket.disconnect();

    return 0;
}