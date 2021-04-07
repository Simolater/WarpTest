#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "HelloTriangleApplication.cpp"

int main() {
    auto app = HelloTriangleApplication();

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}