#include "chatClient.hpp"
#include <iostream>
#include <string>

ChatClient::ChatClient(asio::io_context& io_context, const tcp::resolver::results_type& endpoints,const std::string& name)
    : io_context_(io_context), socket_(io_context),reconnection_timer_(io_context), endpoints_(endpoints),username_(name) {

}

void ChatClient::start() {
 do_connect(endpoints_);
}

void ChatClient::write(const std::string& msg) {
    if (msg.empty()) return;
    //Use asio::post to ensure thread-safety when accessing the queue
    asio::post(io_context_, [this, msg]() {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress) {
            do_write();
        }
    });
}

void ChatClient::do_connect(const tcp::resolver::results_type& endpoints) {
    //Attempt to connect to the server
    asio::async_connect(socket_, endpoints,
        [this, endpoints](std::error_code ec, tcp::endpoint) {
            if (!ec) {
                //Connection successful! Now send the username.
                std::string initial_msg = username_ + "\n";
                asio::async_write(socket_, asio::buffer(initial_msg),
                    [this, endpoints](std::error_code ec, std::size_t /*length*/) {
                        if (!ec) {
                            //Handshake complete, start reading messages
                            do_read();
                        }
                        else {
                            //Failed to send username (connection dropped?)
                            std::cout << "[System] Initial write failed. Retrying..." << std::endl;
                            retry_connection(endpoints);
                        }
                    });
            }
            else {
                //Server not found or refused connection
                std::cout << "[System] Server not found. Retrying in 5 seconds..." << std::endl;
                retry_connection(endpoints);
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
                messages_.push_back(line);
                if (on_message_received)
                    on_message_received();
                //std::cout << line << std::endl;
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
    if (ec == asio::error::eof) {
        std::cout << "\n[System] Connection closed by the server." << std::endl;
    } else {
        std::cout << "\n[System] Lost connection to server: " << ec.message() << " ❌" << std::endl;
    }

    // Shut down the socket and stop the client
    socket_.close();
}

void ChatClient::retry_connection(const tcp::resolver::results_type& endpoints) {
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
