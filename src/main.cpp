#include "server.h"
#include <iostream>

int main() {
    try {
        Server server("127.0.0.1", 6969);
        server.beginListening();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}