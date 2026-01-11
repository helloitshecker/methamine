#pragma once
#include <asio/ip/tcp.hpp>
#include <print>
#include <asio.hpp>
#include <memory>
#include <system_error>
#include <set>
#include <server/clientSession.hpp>

using asio::ip::tcp;

class ChatServer {
public:
    ChatServer(asio::io_context& io_context, short port);

private:
    asio::io_context& io_context_;
    tcp::acceptor acceptor_;

    void start_accept();
    void handle_accept(std::error_code ec, std::shared_ptr<tcp::socket> socket_ptr);

    std::set<std::shared_ptr<ClientSession>> sessions_;
    ChatRoom room_;

};
