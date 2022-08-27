#include "TurtleManiacServer.h"

#include <stdexcept>
#include <string>
#include <thread>
#include <functional>

using namespace server;
using namespace server::network;

server::network::TurtleManiacServer::TurtleManiacServer(const unsigned short port, const common::Console& console)
 : port(port), console(console), is_running(false), running_threads(0) { 
}

void server::network::TurtleManiacServer::start() {
    if(this->running_threads > 0 || this->is_running.exchange(true))
        return;

    this->socket.setBlocking(true);
    if(this->socket.listen(port) != sf::Socket::Done) {
        this->is_running = false;
        throw std::runtime_error("Unable to open port " + std::to_string(port) + "!");
    }
    this->socket.setBlocking(false);

    std::thread packet_handler_thread(&TurtleManiacServer::packet_handler, this);
    std::thread connection_acceptor_thread(&TurtleManiacServer::connection_acceptor, this);
    std::thread connection_listener_thread(&TurtleManiacServer::connection_listener, this);

    packet_handler_thread.detach();
    connection_acceptor_thread.detach();
    connection_listener_thread.detach();

    this->console.log(common::Console::INFO, "Server started. Listening on port " + std::to_string(this->port) + ".");
}

void server::network::TurtleManiacServer::shutdown() {
    if(!this->is_running.exchange(false))
        return;

    this->console.log(common::Console::INFO, "Server shutting down...");

    this->socket.close();

    while(this->running_threads > 0); //wait for threads to shutdown

}

bool server::network::TurtleManiacServer::running() {
    return this->running_threads > 0 || this->is_running;
}
    
server::network::TurtleManiacServer::~TurtleManiacServer() {
    this->shutdown();
}



//private
void server::network::TurtleManiacServer::handle_packet(sf::Packet& to_process) {
    //TODO
}

void server::network::TurtleManiacServer::packet_handler() {
    this->running_threads++;
    this->console.log(common::Console::INFO, "Packet handler started.");

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

    this->console.log(common::Console::INFO, "Packet handler stopped.");
    this->running_threads--;
}

void server::network::TurtleManiacServer::connection_acceptor() {
    this->running_threads++;
    this->console.log(common::Console::INFO, "Connection acceptor started.");

    while(this->is_running) {
        std::unique_ptr<sf::TcpSocket> client = std::make_unique<sf::TcpSocket>();
        
        switch(this->socket.accept(*client.get())) {
            case sf::Socket::Done:
                {
                    client->setBlocking(false);
                    this->console.log(common::Console::INFO, "Client connected with ip " + client->getRemoteAddress().toString());

                    std::lock_guard<std::mutex> client_connections_guard(this->client_connections_mutex);
                    this->client_connections.push_back(std::move(client));
                }
            case sf::Socket::NotReady:
                continue;
            default:
                this->console.log(common::Console::ERROR, "Client connection attempt failed!");
                continue;
        }
    }

    this->console.log(common::Console::INFO, "Connection acceptor stopped.");
    this->running_threads--;
}

void server::network::TurtleManiacServer::connection_listener() {
    this->running_threads++;
    this->console.log(common::Console::INFO, "Connection listener started.");

    while(this->is_running) {
        std::vector<size_t> disconnected_clients;
        
        {
            std::lock_guard<std::mutex> client_connections_guard(this->client_connections_mutex);

            for(size_t i = 0; i < this->client_connections.size(); i++) {
                std::unique_ptr<sf::TcpSocket>& client = this->client_connections[i];
                sf::Packet received_packet;

                switch(client->receive(received_packet)) {
                    case sf::Socket::Done:
                        {
                            std::lock_guard<std::mutex> packet_queue_lock(this->packet_queue_mutex);
                            this->packet_queue.push(received_packet);
                        }
                        break;
                    case sf::Socket::Disconnected:
                    case sf::Socket::Error:
                        disconnected_clients.push_back(i);
                        break;
                    default:
                        break;
                }
            }
        }

        if(!disconnected_clients.empty()) this->cleanup_connections(disconnected_clients);
    }

    this->console.log(common::Console::INFO, "Connection listener stopped.");
    this->running_threads--;
}

void server::network::TurtleManiacServer::cleanup_connections(const std::vector<size_t>& to_remove) {
    std::lock_guard<std::mutex> client_connections_guard(this->client_connections_mutex);
    
    for(auto iter = to_remove.rbegin(); iter != to_remove.rend(); iter++) {
        this->console.log(common::Console::INFO, "Client with ip " + this->client_connections[*iter]->getRemoteAddress().toString() + " disconnected.");
        this->client_connections.erase(this->client_connections.begin() + *iter);
    }
}