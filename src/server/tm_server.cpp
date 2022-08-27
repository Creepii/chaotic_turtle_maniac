#include <iostream>

#include "TurtleManiacServer.h"

void handle_input(server::network::TurtleManiacServer& server, const common::Console& console, std::string input);

int main() {
    common::Console console{};
    server::network::TurtleManiacServer turtle_server(50141, console);

    console.bind_input_handler(std::bind(handle_input, std::ref(turtle_server), std::ref(console), std::placeholders::_1));
    turtle_server.start();

    while(turtle_server.running());

    return 0;
}

void handle_input(server::network::TurtleManiacServer& server, const common::Console& console, std::string input) {
    if(input == "stop") {
        server.shutdown();
    } else {
        console.log(common::Console::ERROR, "Unknown command!");
    }
}