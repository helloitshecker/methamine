#include "clientUI.hpp"
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/color.hpp>

using namespace ftxui;

ChatUI::ChatUI(ChatClient& client) : client_(client) ,screen_(ScreenInteractive::Fullscreen()){
    client_.on_message_received = [this](){
        screen_.PostEvent(Event::Custom);
    };
}

void ChatUI::run() {
    InputOption input_option;
    input_option.on_enter = [&]() {
        // Only send if it's not just whitespace
        if (input_content.find_first_not_of(" \t\n\v\f\r") != std::string::npos) {
            client_.write(input_content);
        }
        input_content.clear(); // Always clear the box after Enter
    };
    Component input_box = Input(&input_content, "type a message", input_option);

    //Create the Main Renderer

    auto main_renderer = Renderer(input_box, [&] {
        Elements message_list;
        const auto& messages = client_.get_messages();
        for (size_t i = 0; i < messages.size(); ++i) {
            auto msg_element = text(messages[i]);
            if (messages[i].find("[SYSTEM]") == 0){
                msg_element = msg_element | color(Color::Yellow) | dim;
            }
            else{
                msg_element = msg_element | color(Color::Green);
            }
            // Check if this is the most recent message
            if (i == messages.size() - 1) {
                    msg_element = msg_element | focus;
                }
                message_list.push_back(msg_element);
        }

        // Header Section
        auto header = vbox({
            text("T E R M I N A L  C H A T") | color(Color::GreenLight) | bold | center,
            text("LOCATION: myroom") | color(Color::Green) | center,
        }) | size(HEIGHT, EQUAL, 3);

        Elements online_elements;
        online_elements.push_back(text("SYSTEM // ONLINE") | color(Color::GreenLight) | bold);
        online_elements.push_back(separator() | color(Color::Green));
        for (const auto& user : client_.get_online_users()) {
                    online_elements.push_back(text(" > " + user) | color(Color::Green));
        }
        auto sidebar = vbox({
            vbox(std::move(online_elements)), // Render the dynamic list
            filler(),
        }) | size(WIDTH, EQUAL, 25) | border | color(Color::Green);

        // Assemble the UI
        return vbox({
            header,
            hbox({
                vbox(std::move(message_list)) | flex | frame | vscroll_indicator,
                sidebar,
            }) | flex,
            hbox({
                text(" root@matrix:~$ ") | color(Color::GreenLight),
                input_box->Render()
            }) | border | color(Color::Green),
        }) | bgcolor(Color::Black);
    });

    screen_.Loop(main_renderer);
}

std::string ChatUI::getUsername() {
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    std::string name;
    bool aborted = false; // Track if the user forced an exit
    auto input = ftxui::Input(&name, "enter handle...");
    auto component = ftxui::CatchEvent(input, [&](ftxui::Event event) {
    // Handle Enter key
    if (event == ftxui::Event::Return) {
        if (name.find_first_not_of(" \t\n\r") != std::string::npos) {
            screen.ExitLoopClosure()();
            return true;
        }
            return true;
        }
    // Handle Ctrl+C or Escape
        if (event == ftxui::Event::Escape) {
            aborted = true;
            screen.ExitLoopClosure()();
            return true;
        }

            return false;
        });

    auto renderer = ftxui::Renderer(component, [&] {
        return ftxui::vbox({
            ftxui::text("--- SYSTEM ACCESS ---") | ftxui::bold | ftxui::center,
            ftxui::filler(),
            ftxui::hbox({
                ftxui::text(" LOGIN: "),
                input->Render() | ftxui::focus | ftxui::color(ftxui::Color::GreenLight),
                }) | ftxui::center,
            ftxui::filler(),
            ftxui::text("Press ESC to exit") | ftxui::dim | ftxui::center,
            }) | ftxui::border | ftxui::color(ftxui::Color::Green);
        });

        screen.Loop(renderer);

        // If they aborted or the name is empty, return something we can check in main
        if (aborted || name.find_first_not_of(" \t\n\r") == std::string::npos) {
            return "";
        }
        return name;
}
