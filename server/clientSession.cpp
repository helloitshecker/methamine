#include "clientSession.hpp"
#include "chatRoom.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <system_error>

ClientSession::ClientSession(tcp::socket socket,ChatRoom& room) : socket_(std::move(socket)),room_(room){

}

void ClientSession::start(){
    room_.join(shared_from_this());
    do_read_name();
}

void ClientSession::do_read(){
    auto self(shared_from_this());

        asio::async_read_until(socket_, read_buffer_, '\n',
            [this, self](std::error_code ec, std::size_t length) {
                if (!ec) {
                    //Extract the string from the stream buffer
                    std::istream is(&read_buffer_);
                    std::string msg;
                    std::getline(is, msg);

                    std::string formatted_msg = name_ + ": " + msg;

                    std::cout << "[Server] Broadcasting: " << formatted_msg << std::endl;
                    room_.deliver(formatted_msg);

                    do_read(); //
                } else {
                    room_.leave(self);
                    std::cout << "Client disconnected" << std::endl;
                }
            });
}

void ClientSession::deliver(const std::string& msg){
    auto self(shared_from_this());

        // Post to the executor to handle the queue safely in the background thread
        asio::post(socket_.get_executor(), [this, self, msg]() {
            bool write_in_progress = !write_msgs_.empty();

            //Append the '\n' so the client's async_read_until can find it
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

void ClientSession::do_read_name() {
    auto self(shared_from_this());
    asio::async_read_until(socket_, read_buffer_, '\n',
        [this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                std::istream is(&read_buffer_);
                std::getline(is, name_); // The first line is the name

                //Now that we have a name, join the room
                room_.join(shared_from_this());

                // Alert everyone that they've arrived
                room_.deliver(name_ + " joined the chat.");

                // Start the regular chat loop
                do_read();
            }
        });
}
