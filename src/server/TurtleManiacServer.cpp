#include "TurtleManiacServer.h"

#include <stdexcept>
#include <string>
#include <thread>
#include <functional>

using namespace server;
using namespace server::network;
using namespace std::chrono_literals;

server::network::TurtleManiacServer::TurtleManiacServer(const unsigned short port, const size_t ticks_per_second)
 : port(port), is_running(false), running_threads(0), update_delta(1000ms / ticks_per_second) { 
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

    common::Console::get_instance().log(common::Console::INFO, "Server started. Listening on port " + std::to_string(this->port) + ".");
}

void server::network::TurtleManiacServer::shutdown() {
    if(!this->is_running.exchange(false))
        return;

    common::Console::get_instance().log(common::Console::INFO, "Server shutting down...");

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
void server::network::TurtleManiacServer::handle_packet(sf::Packet& to_process, const sf::IpAddress& sender) {
    std::lock_guard<std::mutex> client_connections_lock(this->client_connections_mutex);

    for(std::unique_ptr<sf::TcpSocket>& client : this->client_connections) {
        if(client->getRemoteAddress() == sender) continue;
        
        while(client->send(to_process) == sf::Socket::Partial); // to ensure that the packet is not corrupted
    }
}

void server::network::TurtleManiacServer::packet_handler() {
    this->running_threads++;
    common::Console::get_instance().log(common::Console::INFO, "Packet handler started.");

    std::chrono::system_clock::time_point last_update;

    while(this->is_running) {
        last_update = std::chrono::system_clock::now();
      
        std::queue<std::pair<sf::Packet, sf::IpAddress>> to_process;

        {
            std::lock_guard<std::mutex> packet_queue_lock(this->packet_queue_mutex);
        
            if(!this->packet_queue.empty()) {
                std::swap(to_process, this->packet_queue);
            }
        }

        while(!to_process.empty()) {
            this->handle_packet(to_process.front().first, to_process.front().second);
            to_process.pop();
        }

        this->wait_for_next_tick(last_update);
    }

    common::Console::get_instance().log(common::Console::INFO, "Packet handler stopped.");
    this->running_threads--;
}

void server::network::TurtleManiacServer::connection_acceptor() {
    this->running_threads++;
    common::Console::get_instance().log(common::Console::INFO, "Connection acceptor started.");

    std::chrono::system_clock::time_point last_update;

    while(this->is_running) {
        last_update = std::chrono::system_clock::now();

        std::unique_ptr<sf::TcpSocket> client = std::make_unique<sf::TcpSocket>();
        
        switch(this->socket.accept(*client.get())) {
            case sf::Socket::Done:
                {
                    client->setBlocking(false);
                    common::Console::get_instance().log(common::Console::INFO, "Client connected with ip " + client->getRemoteAddress().toString());

                    std::lock_guard<std::mutex> client_connections_guard(this->client_connections_mutex);
                    this->client_connections.push_back(std::move(client));
                }
            case sf::Socket::NotReady:
                continue;
            default:
                common::Console::get_instance().log(common::Console::ERROR, "Client connection attempt failed!");
                continue;
        }

        this->wait_for_next_tick(last_update);
    }

    common::Console::get_instance().log(common::Console::INFO, "Connection acceptor stopped.");
    this->running_threads--;
}

void server::network::TurtleManiacServer::connection_listener() {
    this->running_threads++;
    common::Console::get_instance().log(common::Console::INFO, "Connection listener started.");

    std::chrono::system_clock::time_point last_update;

    while(this->is_running) {
        last_update = std::chrono::system_clock::now();

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
                            this->packet_queue.push(std::make_pair(received_packet, client->getRemoteAddress()));
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

        this->wait_for_next_tick(last_update);
    }

    common::Console::get_instance().log(common::Console::INFO, "Connection listener stopped.");
    this->running_threads--;
}

void server::network::TurtleManiacServer::wait_for_next_tick(const std::chrono::system_clock::time_point last_update) {
    std::this_thread::sleep_until(last_update + this->update_delta);
}

void server::network::TurtleManiacServer::cleanup_connections(const std::vector<size_t>& to_remove) {
    std::lock_guard<std::mutex> client_connections_guard(this->client_connections_mutex);
    
    for(auto iter = to_remove.rbegin(); iter != to_remove.rend(); iter++) {
        common::Console::get_instance().log(common::Console::INFO, "Client with ip " + this->client_connections[*iter]->getRemoteAddress().toString() + " disconnected.");
        this->client_connections.erase(this->client_connections.begin() + *iter);
    }
}