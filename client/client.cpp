#include "chatClient.hpp"
#include <iostream>
#include <thread>

int main(int argc, char* argv[]) {
    try {
        // i hardcoded this , could use argv
        std::string host = "127.0.0.1";
        std::string port = "1234";

        std::string username;
        std::cout << "Enter your username: ";
        std::getline(std::cin, username);

        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, port);

        // Create the client
        ChatClient client(io_context, endpoints,username);
        client.start();

        auto work_guard = asio::make_work_guard(io_context);

        // Spawn the network thread
        std::thread t([&io_context]() { io_context.run(); });

        // Main thread handles the user input
        std::string msg;
        while (std::getline(std::cin, msg)) {
            client.write(msg);
        }

        // Cleanup
        client.close();
        t.join();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
