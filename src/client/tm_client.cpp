#include <SFML/Graphics.hpp>
#include <random>

#include "network/ClientSocket.h"

#include "TextureManager.h"
#include "Animation.h"

// inclusive min and max
int get_random_int(int min, int max) {
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(min, max);
    return uniform_dist(e1);
}

int main() {
    common::Console console{};
    client::network::ClientSocket socket("127.0.0.1", 50141, console);

    socket.connect();

    client::TextureManager texture_manager(console);
    texture_manager.load_directory(RESOURCE_FOLDER);

    sf::RenderWindow window(sf::VideoMode(500, 500), "Chaotic Turtle Maniac", sf::Style::Titlebar | sf::Style::Close);
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

    const std::vector<std::string> tex = {
        "turtle",
        "turtle_hatch_opening",
        "turtle_hatch_open",
        "turtle_hatch_shooting",
        "turtle_hatch_open",
        "turtle_hatch_opening"
    };
    std::vector<int> time = {1000, 300, 300, 100, 300, 300};
    client::Animation turtle{tex, time, texture_manager};
    turtle.set_repeating(true);

    std::vector<sf::Vector2f> powerup_locations;
    sf::Sprite powerup;
    powerup.setTexture(texture_manager.get_texture("powerup"));
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

        float move_dist = 2.0 * speed_boost;
        float pos_off_x = 0.0;
        float pos_off_y = 0.0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            pos_off_y -= move_dist;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            pos_off_y += move_dist;
        }
        if (!(turtle.getPosition().y + pos_off_y >= 0 && turtle.getPosition().y + pos_off_y + turtle.getTexture()->getSize().y <= window.getSize().y)) {
            pos_off_y = 0.0; // cancel if y movement is out of window
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            pos_off_x -= move_dist;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            pos_off_x += move_dist;
        }
        if (!(turtle.getPosition().x + pos_off_x >= 0 && turtle.getPosition().x + pos_off_x + turtle.getTexture()->getSize().x <= window.getSize().x)) {
            pos_off_x = 0.0; // cancel if x movement is out of window
        }

        if(pos_off_x != 0.0 || pos_off_y != 0) {
            sf::Packet position_change_packet;
            position_change_packet << pos_off_x << pos_off_y;
            socket.send_packet(position_change_packet);
        }

        std::queue<sf::Packet> received = socket.get_available_packets();

        while(!received.empty()) {
            float off_x, off_y;
            received.front() >> off_x >> off_y;
            received.pop();

            pos_off_x += off_x;
            pos_off_y += off_y;
        }

        sf::Vector2f new_pos = turtle.getPosition() + sf::Vector2f{pos_off_x, pos_off_y};
        turtle.setPosition(new_pos);

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
        turtle.tick();

        window.display();
    }

    socket.disconnect();

    return 0;
}