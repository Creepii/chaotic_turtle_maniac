#include "ClientSocket.h"

#include <thread>
#include <string>

using namespace client;
using namespace client::network;

client::network::ClientSocket::ClientSocket(const sf::IpAddress& address, const unsigned short port, const common::Console& console)
 : address(address), port(port), console(console), is_running(false), running_threads(0) {
}

client::network::ClientSocket::~ClientSocket() {
    this->disconnect();
}



// public

void client::network::ClientSocket::connect() {
    if(this->is_running.exchange(true) || this->running_threads > 0) {
        return;
    }

    this->socket.setBlocking(true);
    if(this->socket.connect(this->address, this->port) != sf::Socket::Done) {
        this->console.log(common::Console::ERROR, "Unable to connect to " + this->address.toString() + ":" + std::to_string(this->port) + "!");
        return;
    }
    this->socket.setBlocking(false);

    this->console.log(common::Console::INFO, "Connected to " + this->address.toString() + ":" + std::to_string(this->port) + ".");

    std::thread packet_receiver_thread(&ClientSocket::packet_receiver, this);
    std::thread packet_sender_thread(&ClientSocket::packet_sender, this);

    packet_receiver_thread.detach();
    packet_sender_thread.detach();
}

void client::network::ClientSocket::send_packet(const sf::Packet& to_send) {
    std::lock_guard<std::mutex> to_send_lock(this->to_send_mutex);

    this->to_send.push(to_send);
}

std::queue<sf::Packet> client::network::ClientSocket::get_available_packets() {
    std::lock_guard<std::mutex> received_lock(this->received_mutex);

    std::queue<sf::Packet> received;
    std::swap(this->received, received);


    return received;
}

void client::network::ClientSocket::disconnect() {
    if(!this->is_running.exchange(false)) {
        return;
    }

    this->console.log(common::Console::INFO, "Disconnected from server!");
    this->socket.disconnect();

    while(this->running_threads > 0); //wait for threads to shutdown
}



// private

void client::network::ClientSocket::packet_receiver() {
    this->running_threads++;

    while(this->is_running) {
        sf::Packet received_packet;
        if(this->socket.receive(received_packet) == sf::Socket::Done) {
            std::lock_guard<std::mutex> received_lock(this->received_mutex);
            this->received.push(received_packet);
        }
    }

    this->running_threads--;
}

void client::network::ClientSocket::packet_sender() {
    this->running_threads++;

    while(this->is_running) {
        std::queue<sf::Packet> to_send;
        {
            std::lock_guard<std::mutex> to_send_lock(this->to_send_mutex);

            if(!this->to_send.empty()) {
                std::swap(this->to_send, to_send);
            }
        }

        while(!to_send.empty()) {
            while(this->socket.send(to_send.front()) == sf::Socket::Partial); // to ensure that the packet is not corrupted
            to_send.pop();
        }
    }

    this->running_threads--;
}