#include <iostream>

#include "TurtleManiacServer.h"

int main() {
    server::network::TurtleManiacServer turtle_server(50140);

    turtle_server.start();

    while(true);

    return 0;
}