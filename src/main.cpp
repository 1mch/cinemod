#include "app/Application.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    std::cout << "Starting CineMod (Cinema 4D C++ Clone)..." << std::endl;

    CineMod::Application app;
    if (!app.Init()) {
        std::cerr << "Application initialization failed!" << std::endl;
        return 1;
    }

    app.Run();

    std::cout << "CineMod exited cleanly." << std::endl;
    return 0;
}
