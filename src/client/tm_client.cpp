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

    client::TextureManager texture_manager(console);
    texture_manager.load_directory(RESOURCE_FOLDER);

    sf::RenderWindow window(sf::VideoMode(500, 500), "Chaotic Turtle Maniac");
    window.setFramerateLimit(60);
    window.setIcon(texture_manager.get_texture("logo").getSize().x, texture_manager.get_texture("logo").getSize().y, texture_manager.get_texture("logo").copyToImage().getPixelsPtr());
    sf::Sprite sprite;
    sprite.setTexture(texture_manager.get_texture("holy_salad"));

    sf::Sprite background;
    background.setTexture(texture_manager.get_texture("background"));
    background.setScale(
        (float)window.getSize().x / background.getTexture()->getSize().x,
        (float)window.getSize().y / background.getTexture()->getSize().y
    );

    const std::vector<std::string> tex = {"turtle", "turtle_opening", "turtle_open", "turtle_shooting", "turtle_open", "turtle_opening"};
    std::vector<int> time = {1000, 300, 300, 100, 300, 300};
    client::Animation turtle{tex, time, texture_manager};
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

        //float move_dist = window.getSize().x > window.getSize().y ? window.getSize().x / 100.0 : window.getSize().y / 100.0;
        float move_dist = 5.0;

        sf::Vector2f pos_off = sf::Vector2f(0, 0);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            pos_off -= sf::Vector2f(0.0 , move_dist);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            pos_off += sf::Vector2f(0.0 , move_dist);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            pos_off -= sf::Vector2f(move_dist , 0.0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            pos_off += sf::Vector2f(move_dist , 0.0);
        }

        if(pos_off != sf::Vector2f(0, 0)) {
            sf::Packet position_change_packet;
            position_change_packet << pos_off.x << pos_off.y;
            socket.send_packet(position_change_packet);
        }

        std::queue<sf::Packet> received = socket.get_available_packets();

        while(!received.empty()) {
            float off_x, off_y;
            received.front() >> off_x >> off_y;
            received.pop();

            pos_off += sf::Vector2f(off_x, off_y);
        }

        sf::Vector2f new_pos = turtle.getPosition() + pos_off;
        turtle.setPosition(new_pos);

        window.clear();

        window.draw(background);
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