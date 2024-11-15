#include <Aether/Core/Application.hpp>
#include <spdlog/spdlog.h>
#include <iostream>

using namespace Aether;

int main( int argc, char** argv ) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("%^[%T] [%l] %v%$");
    spdlog::info("⠄⠄⣤⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⣤⠄⠄⠄⠄⣤⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄⠄");
    spdlog::info("⠄⣿⣀⣿⠄⠄⠄⣶⣉⣉⣉⣶⠄⠄⠉⣿⠉⠉⠄⠄⣿⠉⠉⠉⣶⠄⠄⣶⣉⣉⣉⣶⠄⠄⣿⠒⠉⠉⠄⠄");
    spdlog::info("⠿⠄⠄⠄⠿⠄⠄⠛⠤⠤⠤⠒⠄⠄⠄⠛⠤⠤⠄⠄⠿⠄⠄⠄⠿⠄⠄⠛⠤⠤⠤⠒⠄⠄⠿⠄⠄⠄⠄⠄");

    Application& app = Application::getInstance();

    try {
        app.Initialize( argc, argv );
        app.Process();
    }catch(std::exception& e) {
        spdlog::error(e.what());
    }

    system("pause");

    return 0;
}