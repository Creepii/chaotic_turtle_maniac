#include <iostream>

#include "TurtleManiacServer.h"

int main() {
    server::network::TurtleManiacServer turtle_server(50141);

    turtle_server.start();

    while(turtle_server.running());

    return 0;
}