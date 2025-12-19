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
        std::string to_send = input_content;
        input_content.clear();

        if (!to_send.empty()) {
            client_.write(to_send);
        }
    };
    Component input_box = Input(&input_content, "type a message", input_option);

    //Create the Main Renderer

    auto main_renderer = Renderer(input_box, [&] {
        // Chat Log Elements
        Elements message_list;
        for (const auto& msg : client_.get_messages()) {
            message_list.push_back(text(msg) | color(Color::Green));
        }

        // Header Section
        auto header = vbox({
            text("T E R M I N A L  C H A T") | color(Color::GreenLight) | bold | center,
            text("LOCATION: myroom") | color(Color::Green) | center,
        }) | size(HEIGHT, EQUAL, 3);

        // Sidebar Section
        auto sidebar = vbox({
            text("SYSTEM // ONLINE") | color(Color::GreenLight) | bold,
            separator() | color(Color::Green),
            text(" > user_1") | color(Color::Green),
            filler(),
            text("SYSTEM // OFFLINE") | color(Color::DarkGreen),
        }) | size(WIDTH, EQUAL, 25) | border | color(Color::Green);

        // Assemble the UI
        return vbox({
            header,
            hbox({
                vbox(std::move(message_list)) | flex | frame,
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

        //Create the input component
        auto input = ftxui::Input(&name, "enter handle...");
        auto component = ftxui::CatchEvent(input, [&](ftxui::Event event) {
            if (event == ftxui::Event::Return && !name.empty()) {
                screen.ExitLoopClosure()();
                return true;
            }
            return false;
        });

        //The important part: Use focused decorator
        auto renderer = ftxui::Renderer(component, [&] {
            return ftxui::vbox({
                ftxui::text("--- SYSTEM ACCESS ---") | ftxui::bold | ftxui::center,
                ftxui::filler(),
                ftxui::hbox({
                    ftxui::text(" LOGIN: "),

                    input->Render() | ftxui::focus | ftxui::color(ftxui::Color::GreenLight),
                }) | ftxui::center,
                ftxui::filler(),
            }) | ftxui::border | ftxui::color(ftxui::Color::Green);
        });

        screen.Loop(renderer);
        return name;
}
