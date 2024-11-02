#include <Aether/Core/Application.hpp>

using namespace ae;

int main( int argc, char** argv ) {

    Application& app = Application::getInstance();

    app.Initialize( argc, argv );

    app.Process();

    app.Terminate();

    return 0;
}