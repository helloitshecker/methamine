#include <server/server.hpp>

#ifdef _WIN32
#include <windows.h>
#include <clocale>
#endif

int main() {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    try {
        std::print("\n\n\n");
        std::print("███╗   ███╗███████╗████████╗██╗  ██╗ █████╗ ███╗   ███╗██╗███╗   ██╗███████╗\n");
        std::print("████╗ ████║██╔════╝╚══██╔══╝██║  ██║██╔══██╗████╗ ████║██║████╗  ██║██╔════╝\n");
        std::print("██╔████╔██║█████╗     ██║   ███████║███████║██╔████╔██║██║██╔██╗ ██║█████╗  \n");
        std::print("██║╚██╔╝██║██╔══╝     ██║   ██╔══██║██╔══██║██║╚██╔╝██║██║██║╚██╗██║██╔══╝  \n");
        std::print("██║ ╚═╝ ██║███████╗   ██║   ██║  ██║██║  ██║██║ ╚═╝ ██║██║██║ ╚████║███████╗\n");
        std::print("╚═╝     ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝╚══════╝\n");
        std::print("\n\n\n");

        asio::io_context io_context;
        auto work_guard = asio::make_work_guard(io_context);
        short port = 6969;

        ChatServer server(io_context, port);
        io_context.run();

    } catch (std::exception& e) {
        std::print("Exception: {}\n", e.what());
    }
    return 0;
}
