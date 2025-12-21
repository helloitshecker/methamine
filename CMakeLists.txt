cmake_minimum_required(VERSION 3.14)

project(terminal_chat
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Fetch FTXUI ----
include(FetchContent)

include_directories(${CMAKE_SOURCE_DIR})

FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v5.0.0
)

FetchContent_MakeAvailable(ftxui)

# ---- Find Asio ----
if (WIN32)
    find_package(asio CONFIG QUIET)
    if (asio_FOUND)
        add_library(asio INTERFACE ALIAS asio::asio)
    else()
        find_path(ASIO_INCLUDE_DIR
            NAMES asio.hpp
            PATHS
                ${CMAKE_SOURCE_DIR}/third_party/asio/include
                ${CMAKE_PREFIX_PATH}/include
                C:/msys64/ucrt64/include
                C:/msys64/mingw64/include [cite: 2]
        )
        if (NOT ASIO_INCLUDE_DIR)
            message(FATAL_ERROR "Asio not found. Install via vcpkg: vcpkg install asio")
        endif()
        add_library(asio INTERFACE)
        target_include_directories(asio INTERFACE ${ASIO_INCLUDE_DIR})
    endif()
else()
    find_path(ASIO_INCLUDE_DIR
        NAMES asio.hpp
        PATHS
            /usr/include
            /usr/local/include
    )

    if (NOT ASIO_INCLUDE_DIR)
        message(FATAL_ERROR "Asio not found. Install with: sudo apt install libasio-dev")
    endif()

    add_library(asio INTERFACE)
    target_include_directories(asio INTERFACE ${ASIO_INCLUDE_DIR})
endif()

# ---- Server ----
add_executable(server
    server/server.cpp
    server/server.hpp
    server/server_main.cpp
    server/clientSession.cpp
    server/clientSession.hpp
    server/chatRoom.hpp
    server/chatRoom.cpp
)
target_link_libraries(server
    PRIVATE
        asio
)

# ---- Client  ----
add_executable(client
    client/client.cpp
    client/chatClient.cpp
    client/chatClient.hpp
    client/clientUI.cpp
    client/clientUI.hpp
)

target_link_libraries(client
    PRIVATE
        asio
        ftxui::screen
        ftxui::dom
        ftxui::component
)

# ---- Platform Specific Libraries ----
if (WIN32)
    target_link_libraries(server PRIVATE ws2_32 mswsock)
    target_link_libraries(client PRIVATE ws2_32 mswsock)
else()
    target_link_libraries(server PRIVATE pthread)
    target_link_libraries(client PRIVATE pthread)
endif()

# ---- Optional warnings ----
if (MSVC)
    target_compile_options(server PRIVATE /W4)
    target_compile_options(client PRIVATE /W4)
else()
    target_compile_options(server PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(client PRIVATE -Wall -Wextra -Wpedantic)
endif()
