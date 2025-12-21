#include <server/server.hpp>

int main() {
    try {
        // 1. Create the io_context
        asio::io_context io_context;
        auto work_guard = asio::make_work_guard(io_context);
        short port = 1234;

        // 3. Create the server object
        ChatServer server(io_context, port);
        io_context.run();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
