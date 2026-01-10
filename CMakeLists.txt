cmake_minimum_required(VERSION 3.14)

project(terminal_chat
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR})

find_package(ftxui)

if (WIN32)
	find_package(asio)
	set(ASIO_TARGET asio)
else()
	set(ASIO_TARGET "")
endif()

add_executable(server
    server/server.cpp
    server/server.hpp
    server/server_main.cpp
    server/clientSession.cpp
    server/clientSession.hpp
    server/chatRoom.hpp
    server/chatRoom.cpp
)
target_link_libraries(server PRIVATE ${ASIO_TARGET})

add_executable(client
    client/client.cpp
    client/chatClient.cpp
    client/chatClient.hpp
    client/clientUI.cpp
    client/clientUI.hpp
)

target_link_libraries(client
    PRIVATE
		${ASIO_TARGET}
        ftxui::screen
        ftxui::dom
        ftxui::component
)

# Threads and sockets
if (WIN32)
    target_link_libraries(server PRIVATE ws2_32 mswsock)
    target_link_libraries(client PRIVATE ws2_32 mswsock)
else()
    target_link_libraries(server PRIVATE pthread)
    target_link_libraries(client PRIVATE pthread)
endif()

# Debugging
# TODO: Make one for Release version
if (MSVC)
    target_compile_options(server PRIVATE /W4)
    target_compile_options(client PRIVATE /W4)
else()
    target_compile_options(server PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(client PRIVATE -Wall -Wextra -Wpedantic)
endif()
