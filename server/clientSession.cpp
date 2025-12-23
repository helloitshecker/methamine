#include "clientSession.hpp"
#include <server/chatRoom.hpp>
#include <iostream>
#include <string>

ClientSession::ClientSession(tcp::socket socket, ChatRoom& room)
    : socket_(std::move(socket)), room_(room), name_("Guest") {}

void ClientSession::start() {
    //Wait for the Login packet before allowing the user to join.
    do_read_name();
}

void ClientSession::do_read_name() {
    auto self(shared_from_this());
    asio::async_read_until(socket_, read_buffer_, '\n',
        [this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                std::istream is(&read_buffer_);
                std::string packet;
                std::getline(is, packet);
                if (!packet.empty() && packet.back() == '\r') packet.pop_back();

                // Check if it's a Login packet
                if (!packet.empty() && static_cast<protocols::MsgType>(packet[0]) == protocols::MsgType::Login) {
                    name_ = packet.substr(1); // Name is everything after the first byte

                    room_.join(shared_from_this());
                    std::cout << "[Server] " << name_ << " identified and joined." << std::endl;

                    room_.deliver(name_ + " joined the chat.");
                    room_.broadcast_user_list();

                    do_read(); // Transition to main chat loop
                } else {
                    // If the first thing user send isn't a login, disconnect them
                    std::cout << "[Server] Invalid handshake from client." << std::endl;
                }
            }
        });
}

void ClientSession::do_read() {
    auto self(shared_from_this());
    asio::async_read_until(socket_, read_buffer_, '\n',
        [this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                std::istream is(&read_buffer_);
                std::string packet;
                std::getline(is, packet);
                if (!packet.empty() && packet.back() == '\r') packet.pop_back();

                if (!packet.empty()) {
                    protocols::MsgType type = static_cast<protocols::MsgType>(packet[0]);
                    std::string payload = packet.substr(1);

                    if (type == protocols::MsgType::Chat) {
                        std::string formatted_msg = name_ + ": " + payload;
                        std::cout << "[Server] Chat from " << name_ << std::endl;
                        room_.deliver(formatted_msg);
                    }
                }
                do_read();
            } else {
                room_.leave(self);
                room_.broadcast_user_list();
                std::cout << "Client disconnected" << std::endl;
            }
        });
}

void ClientSession::deliver(const std::string& msg) {
    auto self(shared_from_this());
    asio::post(socket_.get_executor(), [this, self, msg]() {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg + "\n");

        if (!write_in_progress) {
            do_write();
        }
    });
}

void ClientSession::do_write() {
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(write_msgs_.front()),
        [this, self](std::error_code ec, std::size_t) {
            if (!ec) {
                write_msgs_.pop_front();
                if (!write_msgs_.empty()) {
                    do_write();
                }
            } else {
                room_.leave(self);
            }
        });
}
