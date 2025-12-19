#pragma once

#include <set>
#include <memory>
#include <string>
#include <iostream>

// Forward declaration to avoid circular includes
class ClientSession;

class ChatRoom {
public:
    //  Add a new participant
    void join(std::shared_ptr<ClientSession> session);

    //  Remove a participant (critical for disconnection!)
    void leave(std::shared_ptr<ClientSession> session);

    //  Send a message to everyone in the set
    void deliver(const std::string& msg);

private:
    // Our collection of active sessions
    std::set<std::shared_ptr<ClientSession>> sessions_;
};
