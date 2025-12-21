#pragma once
#include <asio.hpp>
#include <asio/ip/tcp.hpp>
#include <memory>
#include <deque>
#include <string>
#include <server/chatRoom.hpp>

using asio::ip::tcp;

class ClientSession : public std::enable_shared_from_this<ClientSession> {
  public:
  explicit ClientSession(tcp::socket socket,ChatRoom& room);

  void start();

  void deliver(const std::string& msg);
  std::string get_name() const { return name_; }
  private:
    // The socket for this specific client
    tcp::socket socket_;
    ChatRoom& room_;
    asio::streambuf read_buffer_;
    std::deque<std::string> write_msgs_;
    std::string name_; // client username
    // Functions to handle the async loops
    void do_read();
    void do_write();
    void do_read_name();


};
