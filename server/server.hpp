#pragma once
#include <asio/ip/tcp.hpp>
#include <iostream>
#include <asio.hpp>
#include <memory>
#include <system_error>
#include <set>
#include "clientSession.hpp"

using asio::ip::tcp;

class ChatServer {
public:
    // The constructor prototype (declaration)
    ChatServer(asio::io_context& io_context, short port);



private:
asio::io_context& io_context_;
tcp::acceptor acceptor_; // The component that listens for new connections
void start_accept();
void handle_accept(std::error_code ec, std::shared_ptr<tcp::socket> socket_ptr);
std::set<std::shared_ptr<ClientSession>> sessions_;
ChatRoom room_;

};
