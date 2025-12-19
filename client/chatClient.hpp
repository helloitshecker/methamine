#pragma once

#include <asio.hpp>
#include <functional>
#include <string>
#include <deque>
#include <vector>

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

    std::function<void()> on_message_received;

    const std::vector<std::string>& get_messages() const {return messages_;}

private:
    void do_connect(const tcp::resolver::results_type& endpoints);
    void do_read();
    void do_write();
    void handle_error(std::error_code ec);
    void retry_connection(const tcp::resolver::results_type& endpoints);

    asio::io_context& io_context_;
    tcp::socket socket_;
    tcp::resolver::results_type endpoints_;
    asio::streambuf read_buffer_;
    std::deque<std::string> write_msgs_;
    std::string username_;
    asio::steady_timer reconnection_timer_;
    std::vector<std::string> messages_;


};
