#include "TurtleManiacServer.h"

#include <stdexcept>
#include <string>

using namespace server;
using namespace server::network;

server::network::TurtleManiacServer::TurtleManiacServer(const unsigned short port) : is_running(false), port(port) {
}

void server::network::TurtleManiacServer::start() {
    if(this->is_running.exchange(true))
        return;

    if(this->socket.listen(port) != sf::Socket::Done) {
        this->is_running = false;
        throw std::runtime_error("Unable to open port " + std::to_string(port) + "!");
    }

    std::thread packet_handler_thread(&TurtleManiacServer::packet_handler, this);
    std::thread connection_acceptor_thread(&TurtleManiacServer::connection_acceptor, this);
    std::thread connection_listener_thread(&TurtleManiacServer::connection_listener, this);

    packet_handler_thread.detach();
    connection_acceptor_thread.detach();
    connection_listener_thread.detach();
}

void server::network::TurtleManiacServer::shutdown() { //TODO: not completely thread-safe
    if(!this->is_running.exchange(false))
        return;

    this->socket.close();
}
    
server::network::TurtleManiacServer::~TurtleManiacServer() {
    this->shutdown();
}



//private
void server::network::TurtleManiacServer::handle_packet(sf::Packet& to_process) {
    //TODO
}

void server::network::TurtleManiacServer::packet_handler() {
    while(this->is_running) {
        std::queue<sf::Packet> to_process;

        {
            std::lock_guard<std::mutex> packet_queue_lock(this->packet_queue_mutex);
        
            if(!this->packet_queue.empty()) {
                std::swap(to_process, this->packet_queue);
            }
        }

        while(!to_process.empty()) {
            this->handle_packet(to_process.front());
            to_process.pop();
        }
    }
}

void server::network::TurtleManiacServer::connection_acceptor() {
    while(this->is_running) {
        std::unique_ptr<sf::TcpSocket> client = std::make_unique<sf::TcpSocket>();

        if(this->socket.accept(*client.get()) != sf::Socket::Done) {
            //TODO: Debugger log
            continue;
        }
        client->setBlocking(false);

        std::lock_guard<std::mutex> client_connections_guard(this->client_connections_mutex);
        this->client_connections.push_back(std::move(client));
    }
}

void server::network::TurtleManiacServer::connection_listener() {
    while(this->is_running) {
        std::lock_guard<std::mutex> client_connections_guard(this->client_connections_mutex);

        for(std::unique_ptr<sf::TcpSocket>& client : this->client_connections) {
            sf::Packet received_packet;

            if(client->receive(received_packet) == sf::Socket::Done) {
                std::lock_guard<std::mutex> packet_queue_lock(this->packet_queue_mutex);
                this->packet_queue.push(received_packet);
            }
        }
    }
}