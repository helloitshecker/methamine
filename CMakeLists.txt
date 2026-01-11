cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME Methamine)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)
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
    add_compile_options(/utf-8 /MP)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
else()
    target_link_libraries(server PRIVATE pthread)
    target_link_libraries(client PRIVATE pthread)
endif()
