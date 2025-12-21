#include "chatRoom.hpp"
#include <server/clientSession.hpp>

//  Add a session to the set
void ChatRoom::join(std::shared_ptr<ClientSession> session) {
    sessions_.insert(session);
}

//  Remove a session from the set
void ChatRoom::leave(std::shared_ptr<ClientSession> session) {
    sessions_.erase(session);
    std::string goodbye_msg = session->get_name() + " has left the chat.";

        // 3. Print to server console
        std::cout << "[Room] " << goodbye_msg << " Total participants: " << sessions_.size() << std::endl;

        // 4. Tell everyone else
        deliver(goodbye_msg);
}

//  Broadcast to everyone
void ChatRoom::deliver(const std::string& msg) {
    for (auto& session : sessions_) {
        session->deliver(msg);
    }
}
