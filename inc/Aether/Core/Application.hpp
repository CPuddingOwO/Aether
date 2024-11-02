#pragma once

namespace ae {
    class Application {
    public:
        static Application& getInstance();

        void Initialize( int argc, char* argv[] );
        void Process();
        void Terminate();

        Application(const Application&) = delete;
    private:
        Application() = default;
        Application& operator=(const Application&) = delete;
        ~Application();
    };
}