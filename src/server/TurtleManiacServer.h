#pragma once

#include <mutex>
#include <thread>
#include <queue>
#include <atomic>
#include <memory>

#include <SFML/Network.hpp>

namespace server {
    namespace network {
        class TurtleManiacServer {
            private:
                const unsigned short port;

                std::atomic<bool> is_running;

                sf::TcpListener socket;

                std::mutex packet_queue_mutex;
                std::queue<sf::Packet> packet_queue;

                std::mutex client_connections_mutex;
                std::vector<std::unique_ptr<sf::TcpSocket>> client_connections;

                void handle_packet(sf::Packet& to_process);

                void packet_handler();
                void connection_acceptor();
                void connection_listener();
            public:
                TurtleManiacServer(const unsigned short port);
                void start();
                void shutdown();
                ~TurtleManiacServer();
        };        
    }
}