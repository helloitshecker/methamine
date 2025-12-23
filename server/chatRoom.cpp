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
    std::string packet;
    packet.push_back(static_cast<char>(protocols::MsgType::Chat));
    packet.append(msg);

    for (auto& session : sessions_) {
        session->deliver(packet);
    }
}

void ChatRoom::broadcast_user_list() {
    std::string names_csv;
    for (auto& session : sessions_) {
        if (session->get_name() != "Guest") {
            names_csv += session->get_name() + ",";
        }
    }

    std::string packet;
    packet.push_back(static_cast<char>(protocols::MsgType::UserList));
    packet.append(names_csv);

    for (auto& session : sessions_) {
        session->deliver(packet);
    }
}
