#include "Turtle.h"

client::Turtle::Turtle() :
    Animation{TURTLE_ANIMATION_TEXTURES, TURTLE_ANIMATION_TIMES},
    speed_boost{1.0} {
        this->set_repeating(true);
}

void client::Turtle::tick(client::GameState& game_state) {
    // TODO: collisions

    sf::Vector2f pos_off = this->get_turtle_move(game_state);

    this->send_position_change(game_state, pos_off.x, pos_off.y);

    this->setPosition(this->getPosition() + sf::Vector2f{pos_off.x, pos_off.y});
}

sf::Vector2f client::Turtle::get_turtle_move(client::GameState& game_state) {
    float move_dist = 1.0 + this->speed_boost;
    float pos_off_x = 0.0;
    float pos_off_y = 0.0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        pos_off_y -= move_dist;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        pos_off_y += move_dist;
    }
    // turtle can only move to border not further
    if (this->getPosition().y + pos_off_y < 0.0) {
        pos_off_y = 0.0 - this->getPosition().y;
    }
    if (this->getPosition().y + pos_off_y + this->getTexture()->getSize().y > game_state.get_window_height()) {
        pos_off_y = game_state.get_window_height() - this->getPosition().y - this->getTexture()->getSize().y;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        pos_off_x -= move_dist;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        pos_off_x += move_dist;
    }
    // turtle can only move to border not further
    if (this->getPosition().x + pos_off_x < 0.0) {
        pos_off_x = 0.0 - this->getPosition().x;
    }
    if (this->getPosition().x + pos_off_x + this->getTexture()->getSize().x > game_state.get_window_width()) {
        pos_off_x = game_state.get_window_width() - this->getPosition().x - this->getTexture()->getSize().x;
    }

    return sf::Vector2f{pos_off_x, pos_off_y};
}

void client::Turtle::send_position_change(client::GameState& game_state, float off_x, float off_y) {
    if(off_x != 0.0 || off_y != 0.0) {
        sf::Packet position_change_packet;
        position_change_packet << off_x << off_y;
        game_state.get_socket().send_packet(position_change_packet);
    }   
}