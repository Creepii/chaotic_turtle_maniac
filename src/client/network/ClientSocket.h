#pragma once

#include <queue>
#include <mutex>
#include <cstddef>
#include <atomic>

#include <SFML/Network.hpp>

#include "common/Console.h"

namespace client {
    namespace network {
        class ClientSocket {
            private:
                std::atomic<bool> is_running;
                std::atomic<size_t> running_threads;

                const sf::IpAddress address;
                const unsigned short port;

                sf::TcpSocket socket;

                std::mutex to_send_mutex;
                std::queue<sf::Packet> to_send;

                std::mutex received_mutex;
                std::queue<sf::Packet> received;

                void packet_receiver();
                void packet_sender();
            public:
                ClientSocket(const sf::IpAddress& address, const unsigned short port);
                ~ClientSocket();
                void connect();
                void send_packet(const sf::Packet& to_send);
                std::queue<sf::Packet> get_available_packets();
                void disconnect();
        };
    }
}