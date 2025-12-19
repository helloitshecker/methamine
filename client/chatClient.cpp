#include "chatClient.hpp"
#include <iostream>
#include <string>

ChatClient::ChatClient(asio::io_context& io_context, const tcp::resolver::results_type& endpoints,const std::string& name)
    : io_context_(io_context), socket_(io_context), endpoints_(endpoints),username_(name) {

}

void ChatClient::start() {
 do_connect(endpoints_);
}

void ChatClient::write(const std::string& msg) {
    //Use asio::post to ensure thread-safety when accessing the queue
    asio::post(io_context_, [this, msg]() {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg + "\n");
        if (!write_in_progress) {
            do_write();
        }
    });
}

void ChatClient::do_connect(const tcp::resolver::results_type& endpoints) {
    asio::async_connect(socket_, endpoints,
        [this](std::error_code ec, tcp::endpoint) {
            if (!ec) {
                std::string initial_msg = username_ + "\n";
                asio::async_write(socket_, asio::buffer(initial_msg),
                        [this](std::error_code ec, std::size_t /*length*/) {
                            if (!ec) {
                                do_read();
                            }
                        });
            }
        });
}

void ChatClient::do_read() {
    asio::async_read_until(socket_, read_buffer_, '\n',
        [this](std::error_code ec, std::size_t length) {

            if (!ec) {
                std::istream is(&read_buffer_);
                std::string line;
                std::getline(is, line);
                std::cout << line << std::endl;
                do_read();
            } else {
                close();
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
