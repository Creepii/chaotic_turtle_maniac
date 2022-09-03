#include <iostream>
#include <chrono>
#include <thread>

#include "TurtleManiacServer.h"

void handle_input(server::network::TurtleManiacServer& server, std::string input);

int main() {
    using namespace std::chrono_literals;

    server::network::TurtleManiacServer turtle_server(50141);

    common::Console::get_instance().bind_input_handler(std::bind(handle_input, std::ref(turtle_server), std::placeholders::_1));
    turtle_server.start();

    while(turtle_server.running()) {
        std::this_thread::sleep_for(1000ms);
    }

    return 0;
}

void handle_input(server::network::TurtleManiacServer& server, std::string input) {
    if(input == "stop") {
        server.shutdown();
    } else {
        common::Console::get_instance().log(common::Console::ERROR, "Unknown command!");
    }
}