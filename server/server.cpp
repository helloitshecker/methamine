#include "server.hpp"

    ChatServer::ChatServer(asio::io_context& io_context, short port)
        : io_context_(io_context),
          acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {

        std::cout << "Server starting on port " << port << "...\n";
        start_accept();
    }

    void ChatServer::start_accept(){

        std::shared_ptr<tcp::socket> socket_ptr = std::make_shared<tcp::socket>(io_context_);

        acceptor_.async_accept(*socket_ptr,[this,socket_ptr](std::error_code ec){
           handle_accept(ec,socket_ptr);
        });
    }

    void ChatServer::handle_accept(std::error_code ec, std::shared_ptr<tcp::socket> socket_ptr) {
        if (!ec) {
            std::cout << "[Server] New connection detected from: "
                              << socket_ptr->remote_endpoint().address().to_string() << std::endl;
             //Create the session
             auto new_session = std::make_shared<ClientSession>(std::move(*socket_ptr), room_);
             if (new_session){std::cout<<"session created"<<std::endl;}

             // Add them to our "Room"
            sessions_.insert(new_session);

            //Start the session's read loop
            new_session->start();

        } else {
            std::cerr << "Accept error: " << ec.message() << "\n";
        }

        // CRITICAL: Call start_accept() again to keep the server listening!
        start_accept();
    }
