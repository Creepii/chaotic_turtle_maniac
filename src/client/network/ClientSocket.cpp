#include "ClientSocket.h"

using namespace client;
using namespace client::network;

client::network::ClientSocket::ClientSocket(const sf::IpAddress& address, const unsigned short port)
 : address(address), port(port) {
}

client::network::ClientSocket::~ClientSocket() {
}