#include <iostream>

#include "TurtleManiacServer.h"

int main() {
    common::Logger console{};
    server::network::TurtleManiacServer turtle_server(50141, console);

    turtle_server.start();

    while(turtle_server.running());

    return 0;
}