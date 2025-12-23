#pragma once
#include <cstdint>

namespace protocols {
    enum class MsgType : uint8_t {
        Chat = 0x01,      // Normal chat messages
        UserList = 0x02,  // CSV list of online users
        Login = 0x03      // Initial handshake to set username
    };
}
