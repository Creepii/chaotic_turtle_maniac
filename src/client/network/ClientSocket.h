#pragma once

#include <SFML/Network.hpp>

namespace client {
    namespace network {
        class ClientSocket {
            private:
                const sf::IpAddress address;
                const unsigned short port;

                sf::TcpSocket socket;
            public:
                ClientSocket(const sf::IpAddress& address, const unsigned short port);
                ~ClientSocket();
        };
    }
}