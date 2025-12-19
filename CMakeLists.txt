cmake_minimum_required(VERSION 3.10)

project(terminal_chat
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- Find Asio ----
if (WIN32)
    # On Windows, try to find Asio through vcpkg or other package managers
    find_package(asio CONFIG QUIET)
    if (asio_FOUND)
        add_library(asio INTERFACE ALIAS asio::asio)
    else()
        # Fallback to header-only Asio
        find_path(ASIO_INCLUDE_DIR
            NAMES asio.hpp
            PATHS
                ${CMAKE_SOURCE_DIR}/third_party/asio/include
                ${CMAKE_PREFIX_PATH}/include
                C:/msys64/ucrt64/include
                C:/msys64/mingw64/include
        )
        if (NOT ASIO_INCLUDE_DIR)
            message(FATAL_ERROR "Asio not found. Install via vcpkg: vcpkg install asio")
        endif()
        add_library(asio INTERFACE)
        target_include_directories(asio INTERFACE ${ASIO_INCLUDE_DIR})
    endif()
else()
    # Linux/Unix: Find standalone Asio
    find_path(ASIO_INCLUDE_DIR
        NAMES asio.hpp
        PATHS
            /usr/include
            /usr/local/include
    )

    if (NOT ASIO_INCLUDE_DIR)
        message(FATAL_ERROR "Asio not found. Install with: sudo apt install libasio-dev")
    endif()

    # Create an interface target for Asio
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

# Add platform-specific libraries
if (WIN32)
    target_link_libraries(server PRIVATE ws2_32 mswsock)
else()
    target_link_libraries(server PRIVATE pthread)
endif()

# ---- Client ----
add_executable(client
    client/client.cpp
    client/chatClient.cpp
    client/chatClient.hpp
)
target_link_libraries(client
    PRIVATE
        asio
)

# Add platform-specific libraries
if (WIN32)
    target_link_libraries(client PRIVATE ws2_32 mswsock)
else()
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
