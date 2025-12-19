#include "chatClient.hpp"
#include "clientUI.hpp"
#include <asio.hpp>
#include <thread>

int main(int argc, char* argv[]) {
    //Setup Asio basics
    asio::io_context io_context;

    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "1234"); // localhost
    std::string username = ChatUI::getUsername();

    ChatClient client(io_context, endpoints,username);

    client.start();

    ChatUI ui(client);

    std::thread asio_thread([&]() {
        io_context.run();
    });

    ui.run();

    io_context.stop();
    if (asio_thread.joinable()) {
        asio_thread.join();
    }

    return 0;
}
