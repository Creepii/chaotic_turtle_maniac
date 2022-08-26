#include "TurtleManiacServer.h"

#include <stdexcept>
#include <string>
#include <thread>
#include <functional>

using namespace server;
using namespace server::network;

server::network::TurtleManiacServer::TurtleManiacServer(const unsigned short port, common::Logger& console)
 : port(port), console(console), is_running(false), running_threads(0) { 
   this->console.bind_input_handler(std::bind(&TurtleManiacServer::handle_input, this, std::placeholders::_1));
}

void server::network::TurtleManiacServer::start() {
    if(this->running_threads > 0 || this->is_running.exchange(true))
        return;

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

    this->console.log(common::Logger::INFO, "Server started. Listening on port " + std::to_string(this->port) + ".");
}

void server::network::TurtleManiacServer::shutdown() {
    if(!this->is_running.exchange(false))
        return;

    this->console.log(common::Logger::INFO, "Server shutting down...");

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

void server::network::TurtleManiacServer::handle_input(std::string input) {
    if(input == "stop") {
        this->shutdown();
    } else {
        this->console.log(common::Logger::ERROR, "Unknown command!");
    }
}

void server::network::TurtleManiacServer::packet_handler() {
    this->running_threads++;
    this->console.log(common::Logger::INFO, "Packet handler started.");

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

    this->console.log(common::Logger::INFO, "Packet handler stopped.");
    this->running_threads--;
}

void server::network::TurtleManiacServer::connection_acceptor() {
    this->running_threads++;
    this->console.log(common::Logger::INFO, "Connection acceptor started.");

    while(this->is_running) {
        std::unique_ptr<sf::TcpSocket> client = std::make_unique<sf::TcpSocket>();

        switch(this->socket.accept(*client.get())) {
            case sf::Socket::Done:
                {
                    client->setBlocking(false);

                    std::lock_guard<std::mutex> client_connections_guard(this->client_connections_mutex);
                    this->client_connections.push_back(std::move(client));

                    this->console.log(common::Logger::INFO, "Client connected with ip " + client->getRemoteAddress().toString());
                }
            case sf::Socket::NotReady:
                continue;
            default:
                this->console.log(common::Logger::ERROR, "Client connection attempt failed!");
                continue;
        }
    }

    this->console.log(common::Logger::INFO, "Connection acceptor stopped.");
    this->running_threads--;
}

void server::network::TurtleManiacServer::connection_listener() {
    this->running_threads++;
    this->console.log(common::Logger::INFO, "Connection listener started.");

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

    this->console.log(common::Logger::INFO, "Connection listener stopped.");
    this->running_threads--;
}