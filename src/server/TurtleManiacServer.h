#pragma once

#include <mutex>
#include <queue>
#include <atomic>
#include <memory>
#include <vector>

#include <SFML/Network.hpp>

#include "common/Console.h"

namespace server {
    namespace network {
        class TurtleManiacServer {
            private:
                const unsigned short port;

                std::atomic<bool> is_running;
                std::atomic<size_t> running_threads;

                sf::TcpListener socket;

                std::mutex packet_queue_mutex;
                std::queue<std::pair<sf::Packet, sf::IpAddress>> packet_queue;

                std::mutex client_connections_mutex;
                std::vector<std::unique_ptr<sf::TcpSocket>> client_connections;

                const common::Console& console;

                void handle_packet(sf::Packet& to_process, const sf::IpAddress& sender);

                void packet_handler();
                void connection_acceptor();
                void connection_listener();

                void cleanup_connections(const std::vector<size_t>& to_remove);
            public:
                TurtleManiacServer(const unsigned short port, const common::Console& console);
                void start();
                void shutdown();
                bool running();
                ~TurtleManiacServer();
        };        
    }
}