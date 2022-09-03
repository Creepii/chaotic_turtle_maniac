#include "Turtle.h"

client::Turtle::Turtle() :
    Animation{TURTLE_ANIMATION_TEXTURES, TURTLE_ANIMATION_TIMES},
    speed_boost{1.0} {
        this->set_repeating(true);
}

void client::Turtle::tick(client::GameState& game_state) {
    // TODO: collisions

    float move_dist = 2.0 * this->speed_boost;
    float pos_off_x = 0.0;
    float pos_off_y = 0.0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        pos_off_y -= move_dist;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        pos_off_y += move_dist;
    }
    if (!(this->getPosition().y + pos_off_y >= 0 && this->getPosition().y + pos_off_y + this->getTexture()->getSize().y <= game_state.get_window_height())) {
        pos_off_y = 0.0; // cancel if y movement is out of window
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        pos_off_x -= move_dist;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        pos_off_x += move_dist;
    }
    if (!(this->getPosition().x + pos_off_x >= 0 && this->getPosition().x + pos_off_x + this->getTexture()->getSize().x <= game_state.get_window_width())) {
        pos_off_x = 0.0; // cancel if x movement is out of window
    }

    if(pos_off_x != 0.0 || pos_off_y != 0.0) {
        sf::Packet position_change_packet;
        position_change_packet << pos_off_x << pos_off_y;
        game_state.get_socket().send_packet(position_change_packet);
    }

    // move to main()
    std::queue<sf::Packet> received = game_state.get_socket().get_available_packets();

    while(!received.empty()) {
        float off_x, off_y;
        received.front() >> off_x >> off_y;
        received.pop();

        pos_off_x += off_x;
        pos_off_y += off_y;
    }

    sf::Vector2f new_pos = this->getPosition() + sf::Vector2f{pos_off_x, pos_off_y};
    this->setPosition(new_pos);
}