#include <SFML/Graphics.hpp>
#include "TextureManager.h"
#include "Animation.h"

// fraction: 10 = 10th of screen, 5 = 5th of screen
double calc_scale(const sf::RenderWindow& window, const sf::Texture* texture, double fraction) {
    int win_size = window.getSize().x > window.getSize().y ? window.getSize().x : window.getSize().y;
    double sprite_size = win_size / fraction;
    return sprite_size / texture->getSize().x;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(500, 500), "Project setup");
    window.setFramerateLimit(60);
    // TextureManager texture_manager;
    // texture_manager.load_textures();
    window.setIcon(texture_manager.get_texture("logo").getSize().x, texture_manager.get_texture("logo").getSize().y, texture_manager.get_texture("logo").copyToImage().getPixelsPtr());
    sf::Sprite sprite;
    sprite.setTexture(texture_manager.get_texture("holy_salad"));
    sprite.scale(sf::Vector2f(8.0, 8.0));

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

        window.clear();

        window.draw(sprite);
        double scale = calc_scale(window, sprite.getTexture(), 16);
        sprite.setScale(scale, scale);
        sprite.setPosition(window.getSize().x / 2 - sprite.getTexture()->getSize().x * scale / 2, window.getSize().y / 2 - sprite.getTexture()->getSize().y * scale / 2);

        window.draw(turtle);
        turtle.tick();

        window.display();
    }

    return 0;
}