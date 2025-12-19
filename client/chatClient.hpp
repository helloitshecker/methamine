#pragma once

#include <asio.hpp>
#include <string>
#include <deque>

using asio::ip::tcp;

class ChatClient {
public:
    ChatClient(asio::io_context& io_context, const tcp::resolver::results_type& endpoints,const std::string& name);

    //Interface to send messages
    void write(const std::string& msg);

    //Start the async loop
    void start();

    //Close the connection
    void close();

private:
    void do_connect(const tcp::resolver::results_type& endpoints);
    void do_read();
    void do_write();

    asio::io_context& io_context_;
    tcp::socket socket_;
    tcp::resolver::results_type endpoints_;
    asio::streambuf read_buffer_;
    std::deque<std::string> write_msgs_;
    std::string username_;

};
