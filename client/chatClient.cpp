#include "chatClient.hpp"
#include <asio/error.hpp>
#include <iostream>
#include <locale>
#include <string>
#include <common/protocols.hpp>
#include <sstream>

ChatClient::ChatClient(asio::io_context& io_context, const tcp::resolver::results_type& endpoints,const std::string& name)
    : io_context_(io_context), socket_(io_context),reconnection_timer_(io_context), endpoints_(endpoints),username_(name) {

}

void ChatClient::start() {
 do_connect(endpoints_);
}

void ChatClient::write(const std::string& msg) {
    if (msg.empty()) return;

        //Prepend the Chat Header(0x01)
        std::string packet;
        packet.push_back(static_cast<char>(protocols::MsgType::Chat));
        packet.append(msg + "\n");
        asio::post(io_context_, [this, packet]() {
            bool write_in_progress = !write_msgs_.empty();
            write_msgs_.push_back(packet);
            if (!write_in_progress) {
                do_write();
            }
        });
}

void ChatClient::do_connect(const tcp::resolver::results_type& endpoints) {
    asio::async_connect(socket_, endpoints,
        [this, endpoints](std::error_code ec, tcp::endpoint) {
            if (!ec) {
                std::string login_packet;
                login_packet.push_back(static_cast<char>(protocols::MsgType::Login));
                login_packet.append(username_ + "\n");
                asio::async_write(socket_, asio::buffer(login_packet),
                    [this, endpoints](std::error_code ec, std::size_t /*length*/) {
                        if (!ec) {
                            do_read(); // Handshake done, start listening
                        } else {
                            std::cout << "[System] Initial write failed. Retrying..." << std::endl;
                            retry_connection(endpoints);
                        }
                    });
            } else {
                retry_connection(endpoints);
            }
        });
}

void ChatClient::do_read() {
    asio::async_read_until(socket_, read_buffer_, '\n',
        [this](std::error_code ec, std::size_t length) {
            if (!ec) {
                std::istream is(&read_buffer_);
                std::string packet;
                std::getline(is, packet);

                if (!packet.empty() && packet.back() == '\r') packet.pop_back();

                if (!packet.empty()) {
                    protocols::MsgType type = static_cast<protocols::MsgType>(packet[0]);
                    std::string payload = packet.substr(1);

                    if (type == protocols::MsgType::UserList) {
                        online_users_.clear();
                        std::stringstream ss(payload);
                        std::string name;
                        while(std::getline(ss, name, ',')) {
                            if(!name.empty()) online_users_.push_back(name);
                        }
                    }
                    else if (type == protocols::MsgType::Chat) {
                        messages_.push_back(payload);
                    }

                    if (on_message_received) on_message_received();
                }
                do_read();
            } else {
                handle_error(ec);
                retry_connection(endpoints_);
            }
        });
}

void ChatClient::do_write() {
    asio::async_write(socket_, asio::buffer(write_msgs_.front()),
        [this](std::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                write_msgs_.pop_front();
                if (!write_msgs_.empty()) {
                    do_write();
                }
            } else {
                close();
            }
        });
}

void ChatClient::close() {
    asio::post(io_context_, [this]() { socket_.close(); });
}

void ChatClient::handle_error(std::error_code ec) {
    std::string alert = (ec == asio::error::eof)
    ? "[SYSTEM] Connection closed by server"
    : "[SYSTEM] Connection error: " + ec.message();

    messages_.push_back(alert);
    if (on_message_received) on_message_received();

    socket_.close();
}

void ChatClient::retry_connection(const tcp::resolver::results_type& endpoints) {
    messages_.push_back("[SYSTEM] Attempting to reconnect in 5s...");
    //Reset the socket for the next attempt
    std::error_code ignore_ec;
    socket_.close(ignore_ec);

    read_buffer_.consume(read_buffer_.size());

    // Set the timer for a 5-second delay
    reconnection_timer_.expires_after(std::chrono::seconds(5));
    reconnection_timer_.async_wait([this, endpoints](std::error_code ec) {
        if (!ec) {
            do_connect(endpoints); //Loop back to the start
        }
    });
}
